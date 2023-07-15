#include "../include/memoria_config.h"
#include "../../utils/include/funcionesDeMemoria.h"
#include "../../utils/include/list_find_element_and_index.h"
#include "../include/memoria.h"



#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>


#define LOGS_MEMORIA "bin/memoria.log"
#define MODULO_MEMORIA "Memoria"
#define NUMERO_DE_ARGUMENTOS_NECESARIOS 2 
#define COLUMNAS_TABLA_DE_SEGMENTOS 3

extern t_log* memoriaLogger;
extern t_memoria_config* memoriaConfig;

t_list* listaDeHuecosLibres;
t_list* listaDeProcesos;


int socketKERNEL;
int socketCPU;
int socketFilesystem;
uint32_t tamanioRequeridoParaSegmentoACrear;
int tamanioLibreTotal;
int tamanioDeSegmento0;
segmento* pteroASegmento0;
void* bloque_de_memoria;

hueco_libre* (*puntero_algoritmo_asignacion) (void) = NULL;


hueco_libre* obtener_hueco_mas_grande(hueco_libre* hueco1, hueco_libre* hueco2){
    if(hueco1->tamanio > hueco2->tamanio){
        return hueco1;
    }
    else{
        return hueco2;
    }
}

hueco_libre* obtener_hueco_mas_chico(hueco_libre* hueco1, hueco_libre* hueco2){
    if(hueco1->tamanio < hueco2->tamanio){
        return hueco1;
    }
    else{
        return hueco2;
    }
}
bool ver_si_tamanio_requerido_entra_en_hueco_libre(void* huecoAux){
    hueco_libre* hueco = (hueco_libre*) huecoAux;
    return hueco->tamanio >= tamanioRequeridoParaSegmentoACrear;
}

hueco_libre* best_fit(void){ // el hueco + pequeño-- crea peor frag
    return list_get_minimum(listaDeHuecosLibres,(void*) obtener_hueco_mas_chico);
}

hueco_libre* worst_fit(void){ // el hueco + grande 
    return list_get_maximum(listaDeHuecosLibres,(void*) obtener_hueco_mas_grande);
}

hueco_libre* first_fit(void){ // el 1ro que encuentra
    return list_find(listaDeHuecosLibres, ver_si_tamanio_requerido_entra_en_hueco_libre); 
}



void handshake_filesystem(int socketFilesystem){
    uint8_t respuestaFILESYSTEM = stream_recibir_header(socketFilesystem);
    stream_recibir_buffer_vacio(socketFilesystem);
    if (respuestaFILESYSTEM != HANDSHAKE_filesystem) {
        log_error(memoriaLogger, "error al intentar establecer HANDSHAKE inicial con FILESYSTEM");
        log_destroy(memoriaLogger);
        exit(-1);
    }
    stream_enviar_buffer_vacio(socketFilesystem, HANDSHAKE_puede_continuar);
    log_info(memoriaLogger, "conexion con FILESYSTEM establecida");
}

void handshake_cpu(int socketCPU){
    uint8_t respuestaCPU = stream_recibir_header(socketCPU);
    stream_recibir_buffer_vacio(socketCPU);
    if (respuestaCPU != HANDSHAKE_cpu) {
        log_error(memoriaLogger, "error al intentar establecer HANDSHAKE inicial con socketCPU");
        log_destroy(memoriaLogger);
        exit(-1);
    }
    stream_enviar_buffer_vacio(socketCPU, HANDSHAKE_puede_continuar);
    log_info(memoriaLogger, "conexion con CPU establecida");
}

void handshake_kernel(void){
    uint8_t respuestaKERNEL = stream_recibir_header(socketKERNEL);
    stream_recibir_buffer_vacio(socketKERNEL);
    if (respuestaKERNEL != HANDSHAKE_kernel) {
        log_error(memoriaLogger, "error al intentar establecer HANDSHAKE inicial con socketKERNEL");
        log_destroy(memoriaLogger);
        exit(-1);
    }
    stream_enviar_buffer_vacio(socketKERNEL, HANDSHAKE_puede_continuar);
    log_info(memoriaLogger, "conexion con KERNEL establecida");
}

void avisar_si_hay_error(int socket, char* tipo){
    if (socket == -1) {
        log_error(memoriaLogger, "no se pudo establecer conexion inicial con %s", tipo);
        exit(-1);
    }
}


void enviar_cant_segmentos(int SOCKET, char* MENSAJE){
    t_buffer *buffer = buffer_crear();
    int cantidadDeSegmentos = memoria_config_obtener_cantidad_de_segmentos(memoriaConfig);
    buffer_empaquetar(buffer,&cantidadDeSegmentos , sizeof(cantidadDeSegmentos));
    stream_enviar_buffer(SOCKET, HEADER_cantidad_seg_enviada, buffer);
    buffer_destruir(buffer);
    log_info(memoriaLogger, "Se envio la cantidad de segmentos a %s", MENSAJE);
}


void sumar_tamanio_al_libre_total(hueco_libre* hueco){
    tamanioLibreTotal += hueco->tamanio;
}

int obtener_tamanio_libre_total(void){
    tamanioLibreTotal = 0;
    list_iterate(listaDeHuecosLibres, (void*) sumar_tamanio_al_libre_total);
    return tamanioLibreTotal;
}



// hueco_libre* hueco = (hueco_libre*) huecoAux;
void compactar(){

    int cursor = memoria_config_obtener_tamanio_segmento_0(memoriaConfig);
    void* bloque_espejo_para_compactar = malloc(memoria_config_obtener_tamanio_memoria(memoriaConfig));
    memcpy(bloque_espejo_para_compactar, bloque_de_memoria, memoria_config_obtener_tamanio_segmento_0(memoriaConfig));
    void copiarProcesoAListaCompactada(void* Aux){
        proceso* procesoACopiar = (proceso*) Aux;   

        for(int i=1; i<memoria_config_obtener_cantidad_de_segmentos(memoriaConfig); i++){

            if(procesoACopiar->tablaDeSegmentos[i].id != -1){
                memcpy(bloque_espejo_para_compactar + cursor, bloque_de_memoria + procesoACopiar->tablaDeSegmentos[i].base, procesoACopiar->tablaDeSegmentos[i].tamanio);
                procesoACopiar->tablaDeSegmentos[i].base = cursor; 
                cursor += procesoACopiar->tablaDeSegmentos[i].tamanio;
                log_info(memoriaLogger, "PID: %d - Segmento: %d - Base: %d - Tamaño %d", procesoACopiar->pid, procesoACopiar->tablaDeSegmentos[i].id, procesoACopiar->tablaDeSegmentos[i].base, procesoACopiar->tablaDeSegmentos[i].tamanio);
            }
        }
    }
    
    list_iterate(listaDeProcesos, copiarProcesoAListaCompactada); 

    list_clean_and_destroy_elements(listaDeHuecosLibres, free);
    int direccionNuevoHueco = cursor; 
    int tamanioNuevoHueco = memoria_config_obtener_tamanio_memoria(memoriaConfig) - cursor;

    hueco_libre* nuevoHueco = crear_hueco_libre(tamanioNuevoHueco, direccionNuevoHueco);
    list_add(listaDeHuecosLibres, nuevoHueco);
    free(bloque_de_memoria);
    bloque_de_memoria = bloque_espejo_para_compactar;
}

void ocupar_hueco(int pid, int idSegmento){

    hueco_libre *huecoDisponible = puntero_algoritmo_asignacion();
    proceso *procesoEncontrado = encontrar_proceso(pid);
    segmento *segmentoCreado = segmento_crear(idSegmento, huecoDisponible->direccion, tamanioRequeridoParaSegmentoACrear);
    procesoEncontrado->tablaDeSegmentos[idSegmento] = *segmentoCreado;
    if(huecoDisponible->tamanio == segmentoCreado->tamanio){
        bool esHuecoAEliminar(void*huecoAux){
            hueco_libre* hueco = (hueco_libre*) huecoAux;
            return hueco->direccion == huecoDisponible->direccion;  
        }
        int* indiceAQuitar =  malloc(sizeof(*indiceAQuitar));
        list_find_element_and_index(listaDeHuecosLibres, esHuecoAEliminar, indiceAQuitar);
        list_remove(listaDeHuecosLibres, *indiceAQuitar);
        free(indiceAQuitar);
    }
    int tamanioNuevoDeHueco = huecoDisponible->tamanio - segmentoCreado->tamanio;
    huecoDisponible->direccion += segmentoCreado->tamanio;
    huecoDisponible->tamanio = tamanioNuevoDeHueco;
    t_buffer *buffer = buffer_crear();
    buffer_empaquetar_tabla_de_segmentos(buffer, procesoEncontrado->tablaDeSegmentos, memoria_config_obtener_cantidad_de_segmentos(memoriaConfig));
    stream_enviar_buffer(socketKERNEL, HEADER_segmento_creado, buffer);                     
    buffer_destruir(buffer);
}



void atender_create_segment(int pid, int idSegmento){

    if(list_any_satisfy(listaDeHuecosLibres, ver_si_tamanio_requerido_entra_en_hueco_libre)){
        ocupar_hueco(pid, idSegmento);
    }else if(obtener_tamanio_libre_total() >= tamanioRequeridoParaSegmentoACrear){
        stream_enviar_buffer_vacio(socketKERNEL,HEADER_hay_que_compactar);
        log_info(memoriaLogger, "Solicitud de compactacion");
        /// kernel valida que puede compactar TODO
        uint8_t respuestaKernel = stream_recibir_header(socketKERNEL);
        stream_recibir_buffer_vacio(socketKERNEL);
        if(respuestaKernel == HEADER_bueno_compacta){
            compactar();
            t_buffer* bufferProcesos = buffer_crear();
            buffer_empaquetar_lista_de_procesos_de_memoria(bufferProcesos, listaDeProcesos, memoria_config_obtener_cantidad_de_segmentos(memoriaConfig));
            stream_enviar_buffer(socketKERNEL, HEADER_lista_de_tablas_de_segmentos, bufferProcesos);
            // lo siguiente es despues que el kernel vuelva a confirmar la operacion
            ocupar_hueco(pid, idSegmento);
            buffer_destruir(bufferProcesos);
        }
        else{
            log_error(memoriaLogger, "NO recibi confirmacion de compactar, recibido: %d", respuestaKernel);
        }
        
    }else{
        stream_enviar_buffer_vacio(socketKERNEL,HEADER_proceso_terminado_out_of_memory);
    }
    log_info(memoriaLogger, "Tamanio libre total: %d", obtener_tamanio_libre_total());
}


proceso* encontrar_proceso(int pid){
     bool es_proceso(void* procesoAux){
        proceso* procesoN = (proceso*) procesoAux;
        return procesoN->pid == pid;
    }
    return list_find(listaDeProcesos,es_proceso); 
}


void atender_delete_segment(int pid, int idSegmento ){
  
    proceso* procesoEncontrado = encontrar_proceso(pid); 
    hueco_libre* huecoLibre = crear_hueco_libre(procesoEncontrado->tablaDeSegmentos[idSegmento].tamanio, procesoEncontrado->tablaDeSegmentos[idSegmento].base);
    procesoEncontrado->tablaDeSegmentos[idSegmento].base = -1;
    procesoEncontrado->tablaDeSegmentos[idSegmento].id = -1;
    procesoEncontrado->tablaDeSegmentos[idSegmento].tamanio = -1;
    /// si hay huecos libres aledaños, consolidar.
    
    bool esAledanioDeArriba(void* huecoAuxArriba){
        hueco_libre* huecoArriba = (hueco_libre*) huecoAuxArriba;
        return huecoArriba->direccion + huecoArriba->tamanio == huecoLibre->direccion; 
    }

    bool esAledanioDeAbajo(void*huecoAuxAbajo){
        hueco_libre* huecoAbajo = (hueco_libre*) huecoAuxAbajo;
        return huecoLibre->direccion + huecoLibre->tamanio == huecoAbajo->direccion; 
    }

    int* indiceHuecoDeArriba = malloc(sizeof(*indiceHuecoDeArriba));
    
    hueco_libre* huecoAledanioDeArriba=  list_find_element_and_index(listaDeHuecosLibres, esAledanioDeArriba, indiceHuecoDeArriba);

    if(huecoAledanioDeArriba!= NULL){
        huecoLibre->direccion = huecoAledanioDeArriba->direccion; 
        huecoLibre->tamanio += huecoAledanioDeArriba->tamanio;
        list_remove_and_destroy_element(listaDeHuecosLibres,*indiceHuecoDeArriba, free);
    }
    free(indiceHuecoDeArriba);
    
    int* indiceHuecoDeAbajo =  malloc(sizeof(*indiceHuecoDeAbajo));
    hueco_libre* huecoAledanioDeAbajo = list_find_element_and_index(listaDeHuecosLibres, esAledanioDeAbajo, indiceHuecoDeAbajo);
    
    if(huecoAledanioDeAbajo!=NULL){
        huecoLibre->tamanio += huecoAledanioDeAbajo->tamanio;
        list_remove_and_destroy_element(listaDeHuecosLibres,*indiceHuecoDeAbajo,free);
    }
    free(indiceHuecoDeAbajo);

    list_add(listaDeHuecosLibres, huecoLibre);

}

hueco_libre* crear_hueco_libre(int tamanio, int dir){
    hueco_libre* huecoLibre = malloc(sizeof(*huecoLibre));
    huecoLibre->tamanio = tamanio; 
    huecoLibre->direccion = dir; 
    return huecoLibre;
}

void leer_de_memoria(uint32_t cantidadDeBytes, int direccionFisica, int socket, uint8_t header){
    t_buffer* buffer = buffer_crear();
    buffer_empaquetar(buffer,bloque_de_memoria+direccionFisica,cantidadDeBytes);
    stream_enviar_buffer(socket, header, buffer);
    buffer_destruir(buffer);
}

void escribir_en_memoria(t_buffer* buffer, int direccionFisica, uint32_t cantidadDeBytes, uint8_t header, int socket){
    buffer_desempaquetar(buffer,bloque_de_memoria+direccionFisica, cantidadDeBytes);
    stream_enviar_buffer_vacio(socket, header);
}


void recibir_de_cpu(){
    t_buffer* buffer = buffer_crear();
    for(;;){
        uint32_t pID;
        uint32_t nroSegmento;
        uint32_t offset; 
        uint8_t headerRecibido = stream_recibir_header(socketCPU);
        log_info(memoriaLogger, "recibe de cpu: %d", headerRecibido);
        stream_recibir_buffer(socketCPU,buffer);
        buffer_desempaquetar(buffer,&pID,sizeof(pID));
        buffer_desempaquetar(buffer,&nroSegmento,sizeof(nroSegmento));
        buffer_desempaquetar(buffer,&offset, sizeof(offset));
        proceso* proceso = encontrar_proceso(pID); 
        int direccionFisica = proceso->tablaDeSegmentos[nroSegmento].base + offset;
        uint32_t cantidadDeBytes;
        buffer_desempaquetar(buffer,&cantidadDeBytes, sizeof(cantidadDeBytes));
        sleep(memoria_config_obtener_retardo_memoria(memoriaConfig)/1000);
        switch (headerRecibido)
        {   
        case HEADER_valor_de_memoria: 
            log_info(memoriaLogger, "PID: %d - Acción: LEER  - Dirección física: %d - Tamaño: %d - Origen: CPU", pID, direccionFisica, cantidadDeBytes);
            leer_de_memoria(cantidadDeBytes, direccionFisica, socketCPU, HEADER_valor_de_memoria);
            break;
        case HEADER_valor_de_registro:
            log_info(memoriaLogger, "PID: %d - Acción: ESCRIBIR  - Dirección física: %d - Tamaño: %d - Origen: CPU", pID, direccionFisica, cantidadDeBytes);
            escribir_en_memoria(buffer, direccionFisica, cantidadDeBytes, HEADER_OK_puede_continuar, socketCPU);
            break;
        default:
            log_error(memoriaLogger, "error al reconocer header de CPU");
            break;
        }
    }
    buffer_destruir(buffer);
}

void recibir_de_fileSystem(){
    t_buffer* buffer = buffer_crear();
    for(;;){
        uint32_t pID;
        uint32_t nroSegmento;
        uint32_t offset; 
        uint8_t headerRecibido = stream_recibir_header(socketFilesystem);
        stream_recibir_buffer(socketFilesystem,buffer);
        buffer_desempaquetar(buffer,&pID,sizeof(pID));
        buffer_desempaquetar(buffer,&nroSegmento,sizeof(nroSegmento));
        buffer_desempaquetar(buffer,&offset, sizeof(offset));
        proceso* proceso = encontrar_proceso(pID); 
        int direccionFisica = proceso->tablaDeSegmentos[nroSegmento].base + offset;
        uint32_t cantidadDeBytes;
        buffer_desempaquetar(buffer,&cantidadDeBytes, sizeof(cantidadDeBytes));
        sleep(memoria_config_obtener_retardo_memoria(memoriaConfig)/1000);
        switch (headerRecibido)
        {   
        case HEADER_valor_de_memoria: 
            log_info(memoriaLogger, "PID: %d - Acción: LEER  - Dirección física: %d - Tamaño: %d - Origen: FILE SYSTEM", pID, direccionFisica, cantidadDeBytes);
            leer_de_memoria(cantidadDeBytes, direccionFisica, socketFilesystem, HEADER_valor_de_memoria);
            break;
        case HEADER_valor_de_registro:
            log_info(memoriaLogger, "PID: %d - Acción: ESCRIBIR  - Dirección física: %d - Tamaño: %d - Origen: FILE SYSTEM", pID, direccionFisica, cantidadDeBytes);
            escribir_en_memoria(buffer, direccionFisica, cantidadDeBytes, HEADER_OK_puede_continuar, socketFilesystem);
            break;
        default:
            log_error(memoriaLogger, "error al reconocer header de FILE SYSTEM");
            break;
        }
    }
    buffer_destruir(buffer);
}


void recibir_de_kernel(){

    t_buffer * buffer = buffer_crear();

    
    for(; ;){
        uint8_t headerRecibido = stream_recibir_header(socketKERNEL);
        stream_recibir_buffer(socketKERNEL,buffer);
        uint32_t pID;
        buffer_desempaquetar(buffer,&pID,sizeof(pID));
        switch (headerRecibido)
        {
        case HEADER_proceso_a_agregar_a_memoria:
            log_info(memoriaLogger, "Creación de Proceso PID: %d", pID);
            proceso* procesoNuevo = proceso_crear(pID, memoria_config_obtener_cantidad_de_segmentos(memoriaConfig));
            procesoNuevo->tablaDeSegmentos[0] = *pteroASegmento0;
            list_add(listaDeProcesos,procesoNuevo);
            t_buffer* bufferProcesoNuevo = buffer_crear();
            int cantidadDeSegmentos = memoria_config_obtener_cantidad_de_segmentos(memoriaConfig);
            buffer_empaquetar_tabla_de_segmentos(bufferProcesoNuevo,procesoNuevo->tablaDeSegmentos,cantidadDeSegmentos);
            stream_enviar_buffer(socketKERNEL,HEADER_proceso_agregado_a_memoria,bufferProcesoNuevo);
            buffer_destruir(bufferProcesoNuevo);
            break;
        case HEADER_create_segment:
            uint32_t  idSegmento_create;
            buffer_desempaquetar(buffer, &idSegmento_create, sizeof(idSegmento_create)); 
            buffer_desempaquetar(buffer, &tamanioRequeridoParaSegmentoACrear,sizeof(tamanioRequeridoParaSegmentoACrear));
            proceso* procesoDeCreate = encontrar_proceso(pID);
            log_info(memoriaLogger, "PID: %d - Crear Segmento: %d - Base: %d - TAMAÑO: %d", pID, idSegmento_create,procesoDeCreate->tablaDeSegmentos[idSegmento_create].base, procesoDeCreate->tablaDeSegmentos[idSegmento_create].tamanio);
            atender_create_segment(pID,idSegmento_create);
            break;
        case HEADER_delete_segment:
            uint32_t idSegmento_delete; 
            proceso* procesoDeDelete = encontrar_proceso(pID);
            buffer_desempaquetar(buffer,&idSegmento_delete,sizeof(idSegmento_delete));
            log_info(memoriaLogger, "PID: %d - Crear Segmento: %d - Base: %d - TAMAÑO: %d", pID, idSegmento_delete,procesoDeDelete->tablaDeSegmentos[idSegmento_delete].base, procesoDeDelete->tablaDeSegmentos[idSegmento_delete].tamanio);
            atender_delete_segment(pID, idSegmento_delete); 
            break;
        case HEADER_finalizar_proceso_en_memoria:
            log_info(memoriaLogger, "Eliminación de Proceso PID: %d", pID);
            proceso* procesoAFinalizar = encontrar_proceso(pID);
            for(int i=1; i<memoria_config_obtener_cantidad_de_segmentos(memoriaConfig); i++){
                atender_delete_segment(pID, procesoAFinalizar->tablaDeSegmentos[i].id);
            }
            bool esProcesoATerminar(void*procesoAux){
                proceso* procesoATerminar = (proceso*) procesoAux;
                return procesoATerminar->pid == pID; 
            }
            segmento* unSegmento = segmento_crear(0, 0, memoria_config_obtener_tamanio_segmento_0(memoriaConfig));
            procesoAFinalizar->tablaDeSegmentos[0] = *unSegmento;

            int* indiceProcesoAFinalizar =  malloc(sizeof(*indiceProcesoAFinalizar));
            list_find_element_and_index(listaDeProcesos, esProcesoATerminar, indiceProcesoAFinalizar);
            list_remove(listaDeProcesos, *indiceProcesoAFinalizar);  
            // NO ANDA, EL FREE ESTE DE ABAJO ROMPE TODO
            //free(procesoAFinalizar);
            free(indiceProcesoAFinalizar);
            break;
        default:
            log_error(memoriaLogger, "Error al recibir mensaje de KERNEL");
            break;
        }

    }
    buffer_destruir(buffer);
}



int main(int argc, char* argv[]){


    memoriaLogger = log_create(LOGS_MEMORIA, MODULO_MEMORIA, true, LOG_LEVEL_INFO);
    if (argc != NUMERO_DE_ARGUMENTOS_NECESARIOS) {
        log_error(memoriaLogger, "Cantidad de argumentos inválida.\nArgumentos: <configPath>");
        log_destroy(memoriaLogger);
        return -1;
    }
    memoriaConfig = memoria_config_crear(argv[1], memoriaLogger);


    // inicializa servidor de escucha 
    
    int socketESCUCHA = iniciar_servidor("0.0.0.0", memoria_config_obtener_puerto_escucha(memoriaConfig));
    
    struct sockaddr cliente = {0};
    socklen_t len = sizeof(cliente);

    avisar_si_hay_error(socketESCUCHA, "SERVIDOR DE ESCUCHA FILESYSTEM KERNEL CPU");

    log_info(memoriaLogger, "ESPERANDO CLIENTES");

    // acepta conexion con CPU

   socketCPU = accept(socketESCUCHA, &cliente, &len);

    handshake_cpu(socketCPU);

    enviar_cant_segmentos(socketCPU, "CPU");

    // acepta conexion con FILESYSTEM 

    socketFilesystem = accept(socketESCUCHA, &cliente, &len);

    handshake_filesystem(socketFilesystem);

    // acepta conexion con KERNEL

    socketKERNEL= accept(socketESCUCHA, &cliente, &len);

    handshake_kernel();

    enviar_cant_segmentos(socketKERNEL, "KERNEL");
    
    if(memoria_config_es_algoritmo_asignacion_best(memoriaConfig)){
        puntero_algoritmo_asignacion = best_fit;
    }else if(memoria_config_es_algoritmo_asignacion_first(memoriaConfig)){
        puntero_algoritmo_asignacion = first_fit;
    }
    else{
        puntero_algoritmo_asignacion = worst_fit;
    }

    tamanioDeSegmento0 = memoria_config_obtener_tamanio_segmento_0(memoriaConfig);

    bloque_de_memoria = malloc(memoria_config_obtener_tamanio_memoria(memoriaConfig));
    
    pteroASegmento0 = segmento_crear(0,0,memoria_config_obtener_tamanio_segmento_0(memoriaConfig));

    listaDeHuecosLibres = list_create();
    int tamanioDeMemoriaInicial = memoria_config_obtener_tamanio_memoria(memoriaConfig) - tamanioDeSegmento0;
    hueco_libre* huecoLibreInicial = crear_hueco_libre(tamanioDeMemoriaInicial,tamanioDeSegmento0); 
    list_add(listaDeHuecosLibres,huecoLibreInicial);

    listaDeProcesos = list_create(); // al crear pcb
    pthread_t hiloParaEscuchaDeCPU;
    pthread_create(&hiloParaEscuchaDeCPU, NULL, (void*) recibir_de_cpu, NULL);
    pthread_detach(hiloParaEscuchaDeCPU);


    pthread_t hiloParaEscuchaDeFileSystem; 
    pthread_create(&hiloParaEscuchaDeFileSystem, NULL, (void*)recibir_de_fileSystem, NULL);
    pthread_detach(hiloParaEscuchaDeFileSystem);
    
    recibir_de_kernel();
}

