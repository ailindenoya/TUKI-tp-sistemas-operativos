#include "../include/memoria_config.h"
#include "../../utils/include/funcionesDeMemoria.h"
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

int tamanioRequeridoParaSegmentoACrear;
int tamanioLibreTotal;
int tamanioDeSegmento0;
segmento* segmento0;


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

void enviar_cant_segmentos_a_kernel(void){
    t_buffer *buffer = buffer_crear();
    int cantidadDeSegmentos = memoria_config_obtener_cantidad_de_segmentos(memoriaConfig);
    buffer_empaquetar(buffer,&cantidadDeSegmentos , sizeof(cantidadDeSegmentos));
    stream_enviar_buffer(socketKERNEL, HEADER_cantidad_seg_enviada, buffer);
    buffer_destruir(buffer);
    log_info(memoriaLogger, "Se envio la cantidad de segmentos a Kernel");
}


void sumar_tamanio_al_libre_total(hueco_libre* hueco){
    tamanioLibreTotal += hueco->tamanio;
}

int obtener_tamanio_libre_total(void){
    tamanioLibreTotal = 0;
    list_iterate(listaDeHuecosLibres, (void*) sumar_tamanio_al_libre_total);
    return tamanioLibreTotal;
}


/*bool ver_si_tamanio_requerido_entra_en_hueco_libre(void* huecoAux){
    hueco_libre* hueco = (hueco_libre*) huecoAux;
    return hueco->tamanio >= tamanioRequeridoParaSegmentoACrear;
}*/


void atender_create_segment(int pid, int idSegmento){
    
/*
    bool es_proceso(void* procesoAux){
        proceso* procesoN = (proceso*) procesoAux;
        return procesoN->pid == pid;
    }
*/
    if(list_any_satisfy(listaDeHuecosLibres, ver_si_tamanio_requerido_entra_en_hueco_libre)){
        hueco_libre* huecoDisponible = puntero_algoritmo_asignacion();
        proceso* procesoEncontrado =  list_find(listaDeProcesos,es_proceso);
        segmento* segmentoCreado = segmento_crear(idSegmento, huecoDisponible->direccion,tamanioRequeridoParaSegmentoACrear);
        procesoEncontrado->tablaDeSegmentos[idSegmento] = segmentoCreado;
        int tamanioNuevoDeHueco = huecoDisponible->tamanio - segmentoCreado->tamanio; 
        huecoDisponible->tamanio = tamanioNuevoDeHueco;
        t_buffer* buffer = buffer_crear();
        buffer_empaquetar_tabla_de_segmentos(buffer,procesoEncontrado->tablaDeSegmentos,memoria_config_obtener_cantidad_de_segmentos(memoriaConfig));
        stream_enviar_buffer(socketKERNEL,HEADER_segmento_creado,buffer);
        buffer_destruir(buffer);

    }else if(obtener_tamanio_libre_total() >= tamanioRequeridoParaSegmentoACrear){
        // aca compactacion
        
    }else{
        // aca no hay espacio
        stream_enviar_buffer_vacio(socketKERNEL,HEADER_proceso_terminado_out_of_memory);
    }


}

void atender_delete_segment(int pid, int idSegmento ){

}

hueco_libre* crear_hueco_libre(int tamanio, int dir){
    hueco_libre* huecoLibre = malloc(sizeof(*huecoLibre));
    huecoLibre->tamanio = tamanio; 
    huecoLibre->direccion = dir; 
    return huecoLibre;
}


void recibir_de_kernel(void){

    t_buffer * buffer = buffer_crear();
    
    for(; ;){
        uint8_t headerRecibido = stream_recibir_header(socketKERNEL);
        stream_recibir_buffer(socketKERNEL,buffer);
        uint32_t pID;
        buffer_desempaquetar(buffer,&pID,sizeof(pID));

        switch (headerRecibido)
        {
        case HEADER_proceso_a_agregar_a_memoria:
            proceso* procesoNuevo = proceso_crear(pID, memoria_config_obtener_cantidad_de_segmentos(memoriaConfig));
            procesoNuevo->tablaDeSegmentos[0] = segmento0;
            list_add(listaDeProcesos,procesoNuevo);
            t_buffer* bufferProcesoNuevo = buffer_crear();
            int cantidadDeSegmentos = memoria_config_obtener_cantidad_de_segmentos(memoriaConfig);
            buffer_empaquetar_tabla_de_segmentos(bufferProcesoNuevo,procesoNuevo->tablaDeSegmentos,cantidadDeSegmentos);
            buffer_empaquetar_proceso_de_memoria(bufferProcesoNuevo,procesoNuevo,cantidadDeSegmentos);
            stream_enviar_buffer(socketKERNEL,HEADER_proceso_agregado_a_memoria,bufferProcesoNuevo);
            buffer_destruir(bufferProcesoNuevo);
            break;
        case HEADER_create_segment:
            uint32_t  idSegmento_create;
            buffer_desempaquetar(buffer, &idSegmento_create, sizeof(idSegmento_create));  
            buffer_desempaquetar(buffer, &tamanioRequeridoParaSegmentoACrear,sizeof(tamanioRequeridoParaSegmentoACrear));
            atender_create_segment(pID,idSegmento_create);
            break;
        case HEADER_delete_segment:
            uint32_t idSegmento_delete; 
            buffer_desempaquetar(buffer,&idSegmento_delete,sizeof(idSegmento_delete));
            atender_delete_segment(pID, idSegmento_delete); 
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

    int socketCPU = accept(socketESCUCHA, &cliente, &len);

    handshake_cpu(socketCPU);

    // acepta conexion con FILESYSTEM 

    int socketFilesystem = accept(socketESCUCHA, &cliente, &len);

    handshake_filesystem(socketFilesystem);

    // acepta conexion con KERNEL

    socketKERNEL= accept(socketESCUCHA, &cliente, &len);

    handshake_kernel();

    enviar_cant_segmentos_a_kernel();
    
    if(memoria_config_es_algoritmo_asignacion_best(memoriaConfig)){
        puntero_algoritmo_asignacion = best_fit;
    }else if(memoria_config_es_algoritmo_asignacion_first(memoriaConfig)){
        puntero_algoritmo_asignacion = first_fit;
    }
    else{
        puntero_algoritmo_asignacion = worst_fit;
    }

    tamanioDeSegmento0 = memoria_config_obtener_tamanio_segmento_0(memoriaConfig);

    void* bloque_de_memoria = malloc(memoria_config_obtener_tamanio_memoria(memoriaConfig));
    
    segmento0 = segmento_crear(0,0,memoria_config_obtener_tamanio_segmento_0(memoriaConfig));
    
    listaDeHuecosLibres = list_create();
    int tamanioDeMemoriaInicial = memoria_config_obtener_tamanio_memoria(memoriaConfig) - tamanioDeSegmento0;
    hueco_libre* huecoLibreInicial = crear_hueco_libre(tamanioDeMemoriaInicial,tamanioDeSegmento0); 
    list_add(listaDeHuecosLibres,huecoLibreInicial);


    listaDeProcesos = list_create(); // al crear pcb

    pthread_t hiloDeProcesos;
    pthread_create(&hiloDeProcesos, NULL, (void*)recibir_de_kernel, NULL);


}

