extern int cantidadDeSegmentos;
#include "../include/planificador.h"
#include "../include/comunicacionFileSystem.h"
#include "../../utils/include/list_find_element_and_index.h"

extern t_log* kernelLogger;
extern t_kernel_config* kernelConfig;
t_list* tablaArchivosAbiertos;


static int algoritmoConfigurado;

extern int socketMEMORIA;

uint32_t siguientePID;

sem_t gradoDeMultiprogramacion;
sem_t hayPcbsParaAgregarAlSistema;
sem_t dispatchPermitido;

struct tm* tiempo1970;
time_t tiempoLocalActual;


pthread_mutex_t siguientePIDmutex;
pthread_mutex_t controlListaPcbs;

char** arrayDeRecursos;
int* vectorDeInstancias;
int dimensionDeArrayDeRecursos;
bool hayQueReplanificar = true;
bool hayProcesosBloqueadosPorRecursos = false;
t_pcb* ultimoPcbEjecutado;

t_list** pteroAVectorDeListaDeRecursos;
t_list* listaDePcbs;

typedef enum{
    SUCCESS,
    SEG_FAULT,
    OUT_OF_MEMORY,
    WAIT_DE_RECURSO_NO_EXISTENTE,
    SIGNAL_DE_RECURSO_NO_EXISTENTE
} t_motivos_de_finalizacion;

typedef enum {
    ALGORITMO_FIFO,
    ALGORITMO_HRRN
} t_algoritmo;

t_estado* estadoBlocked;
t_estado* estadoExec;
t_estado* estadoExit;
t_estado* estadoNew;
t_estado* estadoReady; 
t_estado* pcbsEsperandoParaIO;

void loggear_cambio_estado(const char* prev, const char* post, int pid) {
    char* cambioDeEstado = string_from_format("\e[1;93m%s->%s\e[0m", prev, post);
    log_info(kernelLogger, "cambio de estado de %s de PCB con ID %d", cambioDeEstado, pid);
    free(cambioDeEstado);
}

uint32_t obtener_siguiente_pid(void) {
    pthread_mutex_lock(&siguientePIDmutex);
    uint32_t nuevoPID = siguientePID++;
    pthread_mutex_unlock(&siguientePIDmutex);
    return nuevoPID;
}


/*                  ALGORITMOS DE PLANIFICACION                              */
/*                            FIFO                                           */

t_pcb* iniciar_fifo(t_estado* estado){
    pthread_mutex_lock(estado_obtener_mutex(estado));
    t_pcb* pcb = estado_desencolar_primer_pcb(estado);
    pthread_mutex_unlock(estado_obtener_mutex(estado));
    return pcb;
}
/*                            HRRN                                          */


void actualizar_tiempo_ejecutado(t_pcb* pcb, uint32_t tiempo){
    uint32_t tiempoActualizado = pcb_obtener_tiempoEjecutado(pcb) + tiempo;
    pcb_setear_tiempoEjecutado(pcb, tiempoActualizado);
}

double siguiente_estimacion(double realAnterior, double estimacionAnterior) {
    double alfa = kernel_config_obtener_hrrn_alfa(kernelConfig);
    return alfa * realAnterior + (1 - alfa) * estimacionAnterior;
}


void actualizar_pcb_por_fin_de_rafaga(t_pcb* pcb){
    double siguienteEstimacion = siguiente_estimacion(pcb_obtener_tiempoEjecutado(pcb),pcb_obtener_estimacion_prox_rafaga(pcb));
    pcb_setear_tiempoEjecutado(pcb,0);
    pcb_setear_estimacion_prox_rafaga(pcb, siguienteEstimacion);
}

double response_ratio(double estimacionDeProxRafaga, double tiempoEsperandoEnReady){
    return (tiempoEsperandoEnReady + estimacionDeProxRafaga)/estimacionDeProxRafaga;
}

t_pcb* mayor_response_ratio(t_pcb* unPcb, t_pcb* otroPcb){

    double tiempoUnPcb = difftime(tiempoLocalActual,pcb_obtener_tiempoDellegadaAReady(unPcb));
    double tiempoOtroPcb = difftime(tiempoLocalActual,pcb_obtener_tiempoDellegadaAReady(otroPcb));

    double responseRatioDeUno = response_ratio(pcb_obtener_estimacion_prox_rafaga(unPcb), tiempoUnPcb);
    double responseRatioDeOtro = response_ratio(pcb_obtener_estimacion_prox_rafaga(otroPcb), tiempoOtroPcb);
    //log_info(kernelLogger,"PCB <ID %d> de RESPONSE RATIO: %f", pcb_obtener_pid(unPcb), responseRatioDeUno);
    //log_info(kernelLogger,"PCB <ID %d> de RESPONSE RATIO: %f", pcb_obtener_pid(otroPcb), responseRatioDeOtro);

    if(responseRatioDeUno > responseRatioDeOtro){
        return unPcb;
    }
    else{
        return otroPcb;
    }
}


t_pcb* iniciar_HRRN(t_estado* estado, double alfa) {
    tiempoLocalActual = time(NULL);
    tiempo1970 = localtime(&tiempoLocalActual);

    t_pcb* pcbElegido = NULL;
    pthread_mutex_lock(estado_obtener_mutex(estado));
    int cantidadPcbsEnLista = list_size(estado_obtener_lista(estado));
    if (cantidadPcbsEnLista == 1) {
        pcbElegido = estado_desencolar_primer_pcb(estado);
    } else if (cantidadPcbsEnLista > 1) {
        pcbElegido = list_get_maximum(estado_obtener_lista(estado), (void*)mayor_response_ratio);
        estado_remover_pcb_de_cola(estado, pcbElegido);
    }
    pthread_mutex_unlock(estado_obtener_mutex(estado));
    return pcbElegido;
}





/*                          FINALIZADOR DE PCBs                        */

void finalizar_proceso(t_pcb* pcb, int motivoDeFinalizacion){
    pcb_setear_estado(pcb, EXIT);
    estado_encolar_pcb_con_semaforo(estadoExit, pcb);
    loggear_cambio_estado("EXEC", "EXIT", pcb_obtener_pid(pcb));
    sem_post(estado_obtener_sem(estadoExit));
    switch (motivoDeFinalizacion)
    {
    case SUCCESS: // caso feliz
        log_info(kernelLogger, "Se finaliza PCB <ID %d> de tamaño %d por motivo: SUCCESS", pcb_obtener_pid(pcb), pcb_obtener_tamanio(pcb));
        break;
    case SEG_FAULT: // caso seg fault
        log_error(kernelLogger, "Se finaliza PCB <ID %d> de tamaño %d por motivo: SEG_FAULT", pcb_obtener_pid(pcb), pcb_obtener_tamanio(pcb));
        break;
    case OUT_OF_MEMORY: 
        log_error(kernelLogger, "Se finaliza PCB <ID %d> de tamaño %d por motivo: OUT_OF_MEMORY", pcb_obtener_pid(pcb), pcb_obtener_tamanio(pcb));
        break;
    case WAIT_DE_RECURSO_NO_EXISTENTE:
        log_error(kernelLogger, "Se finaliza PCB <ID %d> de tamaño %d por motivo: WAIT_DE_RECURSO_NO_EXISTENTE", pcb_obtener_pid(pcb), pcb_obtener_tamanio(pcb));
        break;
    case SIGNAL_DE_RECURSO_NO_EXISTENTE:
        log_error(kernelLogger, "Se finaliza PCB <ID %d> de tamaño %d por motivo: WAIT_DE_RECURSO_NO_EXISTENTE", pcb_obtener_pid(pcb), pcb_obtener_tamanio(pcb));
        break;
    }
}

void finalizar_pcbs_en_hilo_con_exit(void) {
    for (;;) {
        sem_wait(estado_obtener_sem(estadoExit));
        t_pcb* pcbALiberar = estado_desencolar_primer_pcb_con_semaforo(estadoExit);

        t_buffer* bufferParaMemoria = buffer_crear();
        uint32_t idDePcbALiberar = pcb_obtener_pid(pcbALiberar);
        buffer_empaquetar(bufferParaMemoria,&idDePcbALiberar,sizeof(idDePcbALiberar));
        stream_enviar_buffer(kernel_config_obtener_socket_memoria(kernelConfig), HEADER_finalizar_proceso_en_memoria, bufferParaMemoria);
        buffer_destruir(bufferParaMemoria);

        bool esPCBATerminar(void*pcbAux){
                t_pcb* procesoATerminar = (t_pcb*) pcbAux;
                return pcb_obtener_pid(procesoATerminar) == pcb_obtener_pid(pcbALiberar); 
            }
        int* indiceProcesoAFinalizar =  malloc(sizeof(*indiceProcesoAFinalizar));
        list_find_element_and_index(listaDePcbs, esPCBATerminar, indiceProcesoAFinalizar);
        list_remove(listaDePcbs, *indiceProcesoAFinalizar);
        free(indiceProcesoAFinalizar);
        stream_enviar_buffer_vacio(pcb_obtener_socket_consola(pcbALiberar), HEADER_proceso_terminado);
        pcb_destruir(pcbALiberar);
        sem_post(&gradoDeMultiprogramacion);
    }
}

/*                          INICIO DISPOSITIVO I/O                       */



char* concat(const char* s1, const char* s2){
    char* result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void obtenerListaDePids(t_estado* estado){

    int cantidadDePcbs =list_size(estado_obtener_lista(estado));
    char* vector = "";
    char* vec = malloc(sizeof(*vec));

    for(int i=0; i<cantidadDePcbs; i++){
        int pid = pcb_obtener_pid(list_get(estado_obtener_lista(estado),i)); 
        sprintf(vec, "%d", pid);
      /* log_info(kernelLogger, "Cant pcbs %d", cantidadDePcbs);
        log_info(kernelLogger, "Vec %s", vec);
        log_info(kernelLogger, "Vector %s", vector);*/ 
        vector = concat(vector, vec);
    }

    char* algoritmoPlanif = kernel_config_obtener_algoritmo(kernelConfig);
    log_info(kernelLogger, "Cola Ready %s   %s ", algoritmoPlanif, vector);
    free(vec);
    free(vector);
}

void iniciar_io(t_pcb* pcb) {
   
    log_info(kernelLogger, "Ejecutando ráfagas I/O de PCB <ID %d> por %u milisegundos", pcb_obtener_pid(pcb), pcb_obtener_tiempo_bloqueo(pcb));
      
    intervalo_de_pausa(pcb_obtener_tiempo_bloqueo(pcb));

    pcb_setear_estado(pcb, READY);
    estado_encolar_pcb_con_semaforo(estadoReady, pcb);
    pcb_setear_tiempo_bloqueo(pcb, 0);
    
    loggear_cambio_estado("BLOCKED", "READY", pcb_obtener_pid(pcb));
    pcb_setear_tiempoDellegadaAReady(pcb);
    
    obtenerListaDePids(estadoReady);
    sem_post(estado_obtener_sem(estadoReady));
}
/*                  PLANIFICADOR A LARGO PLAZO                              */

void  planificador_largo_plazo(void) {
    pthread_t liberarPcbsEnExitHilo;
    pthread_create(&liberarPcbsEnExitHilo, NULL, (void*)finalizar_pcbs_en_hilo_con_exit, NULL);
    pthread_detach(liberarPcbsEnExitHilo);
    for (;;) {
        sem_wait(&hayPcbsParaAgregarAlSistema);
        sem_wait(&gradoDeMultiprogramacion);

        pthread_mutex_lock(estado_obtener_mutex(estadoNew));
        t_pcb* pcbQuePasaAReady = list_remove(estado_obtener_lista(estadoNew), 0);
        pthread_mutex_unlock(estado_obtener_mutex(estadoNew));
        
        pthread_mutex_lock(&controlListaPcbs);
        list_add(listaDePcbs, pcbQuePasaAReady);
        pthread_mutex_unlock(&controlListaPcbs);
        ////// PARTE DE MEMORIA 
        avisar_a_memoria_de_crear_segmentos_de_proceso(pcbQuePasaAReady);
        
        t_buffer* bufferSegmentoCreado = buffer_crear();
        uint8_t respuestaDeMemoria = stream_recibir_header(kernel_config_obtener_socket_memoria(kernelConfig));
        log_info(kernelLogger, "kernel OBTIENE rsta de memoria %d", respuestaDeMemoria);
        if (respuestaDeMemoria != HEADER_proceso_agregado_a_memoria) {
            log_error(kernelLogger, "Error al intentar recibir la tabla de segmentos de MEMORIA <socket %d> para proceso de ID %d", kernel_config_obtener_socket_memoria(kernelConfig), pcb_obtener_pid(pcbQuePasaAReady));
            exit(-1);
        }
        stream_recibir_buffer(kernel_config_obtener_socket_memoria(kernelConfig), bufferSegmentoCreado);
        buffer_desempaquetar_tabla_de_segmentos(bufferSegmentoCreado,pcb_obtener_tabla_de_segmentos(pcbQuePasaAReady),cantidadDeSegmentos);
        buffer_destruir(bufferSegmentoCreado);

        log_info(kernelLogger, "Proceso con ID %d tiene ahora su segmento 0 de tamanio %d cargado en MEMORIA ", pcb_obtener_pid(pcbQuePasaAReady), pcb_obtener_tabla_de_segmentos(pcbQuePasaAReady)[0].tamanio);
        ///////////
        
        estado_encolar_pcb_con_semaforo(estadoReady, pcbQuePasaAReady);
        loggear_cambio_estado("NEW", "READY", pcb_obtener_pid(pcbQuePasaAReady));
        obtenerListaDePids(estadoReady);
        pcb_setear_tiempoDellegadaAReady(pcbQuePasaAReady);
        sem_post(estado_obtener_sem(estadoReady));

        pcbQuePasaAReady = NULL;
    }
}
/*                  PLANIFICADOR A CORTO PLAZO                             */

uint32_t obtener_tiempo_en_milisegundos(struct timespec end, struct timespec start) {
    const uint32_t SECS_TO_MILISECS = 1000;
    const uint32_t NANOSECS_TO_MILISECS = 1000000;
    return (end.tv_sec - start.tv_sec) * SECS_TO_MILISECS + (end.tv_nsec - start.tv_nsec) / NANOSECS_TO_MILISECS;
} 


void atender_bloqueo(t_pcb* pcb) {
  //  pcb_marcar_tiempo_inicial_bloqueado(pcb); PREGUNTA
    loggear_cambio_estado("EXEC", "BLOCKED", pcb_obtener_pid(pcb));
    log_info(kernelLogger, "PID: %d - Bloqueado por: IO ", pcb_obtener_pid(pcb));
    pcb_setear_estado(pcb, BLOCKED);
    pthread_t operacionIO;
    pthread_create(&operacionIO, NULL, (void*) iniciar_io , pcb);
    pthread_detach(operacionIO);
}

static void __set_timespec(struct timespec* timespec) {
    int retVal = clock_gettime(CLOCK_REALTIME, timespec);
    if (retVal == -1) {
        perror("clock_gettime");
        exit(-1);
    }
}


int obtenerDimensionDeArrayDeRecursos(char** instancias){
    int contador = 0;
    char** pteroAinstancia = instancias; 
    while(*pteroAinstancia!=NULL){
        contador++;
        pteroAinstancia++;
    }
    return contador;
}

int* convertirInstanciasDeRecursoEnEnteros(char** instancias, int dimension){
    int* vectorAux = malloc(sizeof(int)* dimension);
    for(int i=0; i<dimension; i++){
        vectorAux[i] = atoi(instancias[i]);
    }
    return vectorAux;
}


void atender_wait(char* recurso, t_pcb* pcb){
    
    char** pteroARecursos = kernel_config_obtener_recursos(kernelConfig);
    int i;
    for(i=0; i<dimensionDeArrayDeRecursos;i++){
        if(strcmp(*pteroARecursos, recurso) == 0){
            vectorDeInstancias[i]--;
            if(vectorDeInstancias[i] < 0){
                list_add(pteroAVectorDeListaDeRecursos[i],pcb);
                pcb_setear_estado(pcb, BLOCKED);
                loggear_cambio_estado("EXEC", "BLOCKED", pcb_obtener_pid(pcb));
                log_info(kernelLogger, "PID: %d - Bloqueado por: %s ", pcb_obtener_pid(pcb), recurso);
                //hayProcesosBloqueadosPorRecursos = true;
                hayQueReplanificar = true;
            }
            else{
                hayQueReplanificar = false;
            }
            break; 
        }
        pteroARecursos++;
    }

    if(i == dimensionDeArrayDeRecursos){
            finalizar_proceso(pcb,WAIT_DE_RECURSO_NO_EXISTENTE);
            hayQueReplanificar = true; 
    }
    loggearInstancias(pcb, recurso);
}

void loggearInstancias(t_pcb* pcb, char* recurso){
    char inst[dimensionDeArrayDeRecursos]; 
    for(int i=0; i<dimensionDeArrayDeRecursos; i++){
        inst[i] = vectorDeInstancias[i];
    }
    log_info(kernelLogger, "PID: %d - Signal: %s - Instancias: %s ", pcb_obtener_pid(pcb), recurso, inst);
}

void atender_signal(char* recurso, t_pcb* pcb){
    
    char** pteroARecursos = kernel_config_obtener_recursos(kernelConfig);
    int i;
    for(i=0; i<dimensionDeArrayDeRecursos;i++){
        if(strcmp(*pteroARecursos, recurso) == 0){
            vectorDeInstancias[i]++;
            if(vectorDeInstancias[i] == 0){
                t_pcb* pcbADesbloquear = list_get(pteroAVectorDeListaDeRecursos[i], 0);  //agarra el primero de la cola de bloqueados del recurso
                list_remove(pteroAVectorDeListaDeRecursos[i],0);
                pcb_setear_estado(pcbADesbloquear, READY);
                estado_encolar_pcb_con_semaforo(estadoReady, pcbADesbloquear);
                loggear_cambio_estado("BLOCKED", "READY", pcb_obtener_pid(pcbADesbloquear));
                obtenerListaDePids(estadoReady);
                pcb_setear_tiempoDellegadaAReady(pcbADesbloquear);
                sem_post(estado_obtener_sem(estadoReady));
            }
            break; 
        }
        pteroARecursos++;
    }
    if(i == dimensionDeArrayDeRecursos){
            finalizar_proceso(pcb,SIGNAL_DE_RECURSO_NO_EXISTENTE);
            hayQueReplanificar = true;
    }else{
        hayQueReplanificar = false;
    }
    loggearInstancias(pcb, recurso);
}

t_archivo_tabla* encontrarEntradaEnTablaGlobal(char* nombreArchivo){

    bool encontrarArch(void* Aux){
        t_archivo_tabla* tab = (t_archivo_tabla*) Aux; 
                    return strcmp(t_archivo_tabla_obtener_nombre_archivo(tab), nombreArchivo)== 0;
    }
    return list_find(tablaArchivosAbiertos, encontrarArch); 

}

t_archivo_tabla_proceso* encontrarArchivoTablaProcesos(char* nombreArchivo, t_pcb* pcb){
   t_list* tablaArchivosAbiertos = pcb_obtener_tabla_de_archivos_abiertos(pcb);
   bool encontrarArch(void* Aux){
        t_archivo_tabla_proceso* tab = (t_archivo_tabla_proceso*) Aux; 
                    return strcmp(t_archivo_tabla_proceso_obtener_nombre_archivo(tab), nombreArchivo)== 0;
    }
    return list_find(tablaArchivosAbiertos, encontrarArch);  
}

void enviar_F_OPEN_a_FS(char* nombreArchivoNuevo, uint32_t pid){
    int socket_fs_peticiones = kernel_config_obtener_socket_filesystem_peticiones(kernelConfig);
    t_buffer* buffer_F_OPEN = buffer_crear();
    buffer_empaquetar_string(buffer_F_OPEN, nombreArchivoNuevo);
    stream_enviar_buffer(socket_fs_peticiones, HEADER_F_OPEN, buffer_F_OPEN);

    uint8_t respuestaFileSystem = stream_recibir_header(socket_fs_peticiones);
    stream_recibir_buffer_vacio(socket_fs_peticiones);

    if (respuestaFileSystem == HEADER_no_existe_archivo){
        stream_enviar_buffer_vacio(socket_fs_peticiones, HEADER_crear_archivo);
        respuestaFileSystem = stream_recibir_header(socket_fs_peticiones);
        stream_recibir_buffer_vacio(socket_fs_peticiones);
        log_info(kernelLogger, "no existe el archivo");
        if(respuestaFileSystem == HEADER_archivo_abierto){
            t_archivo_tabla* entradaDeTabla = crearEntradaEnTabla(pid, nombreArchivoNuevo);
            list_add(tablaArchivosAbiertos, (void*) entradaDeTabla);
            log_info(kernelLogger, "no existe, creo el archivo");
        }
        else {
            log_error(kernelLogger, "Error al abrir el archivo: %s", nombreArchivoNuevo);
        }
    }
    else if (respuestaFileSystem == HEADER_archivo_abierto){
        t_archivo_tabla* entradaDeTabla = crearEntradaEnTabla(pid, nombreArchivoNuevo);
        list_add(tablaArchivosAbiertos, (void*) entradaDeTabla);
        log_info(kernelLogger, "abrio el archivo");
    }
    buffer_destruir(buffer_F_OPEN);
}


t_pcb* encontrar_pcb(int pid){
    bool es_proceso(void* procesoAux){
        t_pcb* procesoN = (t_pcb*) procesoAux;
        return procesoN->pid == pid;
    }
    pthread_mutex_lock(&controlListaPcbs);
    t_pcb* unPcb = list_find(listaDePcbs,es_proceso); 
    pthread_mutex_unlock(&controlListaPcbs);
    return unPcb;
}


void buffer_desempaquetar_y_actualizar_lista_procesos(t_buffer* bufferProcesos){ //CARRI SEAL OF APPROVAL
    int cantidad;
    buffer_desempaquetar(bufferProcesos, &cantidad, sizeof(cantidad));
    
    for (int i = 0; i < cantidad; i++){
        int pid;
        buffer_desempaquetar(bufferProcesos, &pid, sizeof(pid));
        t_pcb* pcbAActualizarTabla = encontrar_pcb(pid);
        pthread_mutex_lock(pcb_obtener_mutex(pcbAActualizarTabla));
        buffer_desempaquetar_tabla_de_segmentos(bufferProcesos, pcb_obtener_tabla_de_segmentos(pcbAActualizarTabla), cantidadDeSegmentos);
        pthread_mutex_unlock(pcb_obtener_mutex(pcbAActualizarTabla));
    }
}

void desbloquearProcesoDesdeFS(char* nombreArchivo) {
    t_archivo_tabla* entradaTablaGlobal = encontrarEntradaEnTablaGlobal(nombreArchivo);
    uint32_t pid = t_archivo_tabla_obtener_pid(entradaTablaGlobal);
    t_pcb* pcbADesbloquear = encontrar_pcb(pid);
    pcb_setear_estado(pcbADesbloquear, READY);
    estado_encolar_pcb_con_semaforo(estadoReady, pcbADesbloquear);
    loggear_cambio_estado("EXEC", "READY", pcb_obtener_pid(pcbADesbloquear));
    pcb_setear_tiempoDellegadaAReady(pcbADesbloquear);
    sem_post(estado_obtener_sem(estadoReady));
}

void atenderBloqueoDe_Filesystem(){
    int socketFSDesbloqueos = kernel_config_obtener_socket_filesystem_desbloqueos(kernelConfig);
    for(;;) {
        t_buffer* bufferDesbloqueo = buffer_crear();
        uint8_t headerFS = stream_recibir_header(socketFSDesbloqueos);
        char* nombreArchivo = malloc(sizeof(*nombreArchivo));
        stream_recibir_buffer(socketFSDesbloqueos, bufferDesbloqueo);
        buffer_desempaquetar_string(bufferDesbloqueo, &nombreArchivo);
        if(headerFS == HEADER_desbloquear_proceso){
            desbloquearProcesoDesdeFS(nombreArchivo);
            break;
        }else{
            log_error(kernelLogger, "No reconoce header desde FS desbloqueos");
            exit(-1);
            break;
        }
        buffer_destruir(bufferDesbloqueo);
        free(nombreArchivo);
    }
}

void atender_pcb() {
    int socketMemoria = kernel_config_obtener_socket_memoria(kernelConfig);

    for (;;) {
        sem_wait(estado_obtener_sem(estadoExec)); 
        
        pthread_mutex_lock(estado_obtener_mutex(estadoExec));
        
        t_pcb* pcb = list_get(estado_obtener_lista(estadoExec), 0); // saca el primer pcb de la lista
        pthread_mutex_unlock(estado_obtener_mutex(estadoExec));
        
        if(hayQueReplanificar == true){
            loggear_cambio_estado("READY", "EXEC", pcb_obtener_pid(pcb));
        }
        // tiempo real ejecutado - rafaga
        struct timespec start;  
        __set_timespec(&start);

        kernel_enviar_pcb_a_cpu(pcb, kernelConfig, kernelLogger, HEADER_proceso_a_ejecutar);
        uint8_t cpuRespuesta = stream_recibir_header(kernel_config_obtener_socket_cpu(kernelConfig));
        log_info(kernelLogger, "HEADER DE RSTA CPU: %d", cpuRespuesta);
        struct timespec end;
        __set_timespec(&end);
        // tiempo real ejecutado - rafaga

        pthread_mutex_lock(estado_obtener_mutex(estadoExec));
        pcb = kernel_recibir_pcb_actualizado_de_cpu(pcb, cpuRespuesta, kernelConfig, kernelLogger);

        ultimoPcbEjecutado = pcb; // que pcb sigue ejecutando
        
        list_remove(estado_obtener_lista(estadoExec), 0); // saca de ejec el proceso

        pthread_mutex_unlock(estado_obtener_mutex(estadoExec));

        uint32_t realEjecutado = 0;
        realEjecutado = obtener_tiempo_en_milisegundos(end, start); 

        log_debug(kernelLogger, "PCB <ID %d> estuvo en ejecución por %d milisegundos", pcb_obtener_pid(pcb), realEjecutado);       
        
        actualizar_tiempo_ejecutado(pcb, realEjecutado);

        uint32_t pidAEnviar = pcb_obtener_pid(pcb);
        
        switch (cpuRespuesta) {
            case HEADER_proceso_terminado_seg_fault:
                finalizar_proceso(pcb,SEG_FAULT);
                hayQueReplanificar = true; 
                break;
            case HEADER_proceso_terminado:
                finalizar_proceso(pcb,SUCCESS);
                registros_imprimir(pcb_obtener_registros(pcb));
                hayQueReplanificar = true; 
                break;
            case HEADER_proceso_bloqueado:     
                    //EN FIFO NO SE HACE NADA   
                atender_bloqueo(pcb);   // en ambos se atiende el bloqueo
                hayQueReplanificar = true; 
                break;        
            case HEADER_proceso_wait: 
                t_buffer* bufferWAIT = buffer_crear();
                stream_recibir_header(kernel_config_obtener_socket_cpu(kernelConfig));
                stream_recibir_buffer(kernel_config_obtener_socket_cpu(kernelConfig),bufferWAIT);
                char* recursoDesempaquetadoWAIT = malloc(sizeof(*recursoDesempaquetadoWAIT));
                buffer_desempaquetar_string(bufferWAIT, &recursoDesempaquetadoWAIT);
                atender_wait(recursoDesempaquetadoWAIT,pcb);
                buffer_destruir(bufferWAIT);
                free(recursoDesempaquetadoWAIT);
                break;
            case HEADER_proceso_signal:
                t_buffer* bufferSIGNAL = buffer_crear();
                stream_recibir_header(kernel_config_obtener_socket_cpu(kernelConfig));
                stream_recibir_buffer(kernel_config_obtener_socket_cpu(kernelConfig),bufferSIGNAL);
                char* recursoDesempaquetadoSIGNAL = malloc(sizeof(*recursoDesempaquetadoSIGNAL));
                buffer_desempaquetar_string(bufferSIGNAL, &recursoDesempaquetadoSIGNAL);   
                atender_signal(recursoDesempaquetadoSIGNAL,pcb);
                buffer_destruir(bufferSIGNAL);
                free(recursoDesempaquetadoSIGNAL);
                break;
            case HEADER_proceso_yield:
                pcb_setear_estado(pcb, READY);
                estado_encolar_pcb_con_semaforo(estadoReady, pcb);
                loggear_cambio_estado("EXEC", "READY", pcb_obtener_pid(pcb));
                pcb_setear_tiempoDellegadaAReady(pcb);
                sem_post(estado_obtener_sem(estadoReady));
                hayQueReplanificar = true; 
                break;
            case HEADER_proceso_F_OPEN:
                t_buffer* buffer_F_OPEN = buffer_crear();
                int recibidoDeCPU = stream_recibir_header(kernel_config_obtener_socket_cpu(kernelConfig));
                if(recibidoDeCPU!= HEADER_proceso_parametros){
                    log_error(kernelLogger,"No se recibio el mensaje de CPU");
                    exit(-1);
                }
                stream_recibir_buffer(kernel_config_obtener_socket_cpu(kernelConfig), buffer_F_OPEN);
                char* nombreArchivoNuevo = malloc(sizeof(*nombreArchivoNuevo));
                buffer_desempaquetar_string(buffer_F_OPEN, &nombreArchivoNuevo);
                
                if(list_is_empty(tablaArchivosAbiertos)){   // Si la tabla está vacía
                    enviar_F_OPEN_a_FS(nombreArchivoNuevo,pcb_obtener_pid(pcb));
                    log_info(kernelLogger, "se envio FOPEN a fs");
                    t_archivo_tabla_proceso* entradaDeTablaDeProceso = crearEntradaEnTablaProceso(nombreArchivoNuevo);
                    pcb_agregar_a_tabla_de_archivos_abiertos(pcb, entradaDeTablaDeProceso); 
                    log_info(kernelLogger, "PID: %d - Abrir Archivo: %s", pcb_obtener_pid(pcb), nombreArchivoNuevo);
        
                    hayQueReplanificar = false;
                    break;
                }
                
                // Tabla no vacía, hay archivos abiertos

                t_archivo_tabla* tablaDeArchivoBuscado = encontrarEntradaEnTablaGlobal(nombreArchivoNuevo);  //Buscas el archivo en la tabla

                if(strcmp(t_archivo_tabla_obtener_nombre_archivo(tablaDeArchivoBuscado),nombreArchivoNuevo)!= 0){    // Si no está, lo abris
                    enviar_F_OPEN_a_FS(nombreArchivoNuevo, pcb_obtener_pid(pcb));
                    t_archivo_tabla_proceso* entradaDeTablaDeProceso = crearEntradaEnTablaProceso(nombreArchivoNuevo);
                    pcb_agregar_a_tabla_de_archivos_abiertos(pcb, entradaDeTablaDeProceso);
                    log_info(kernelLogger, "PID: %d - Abrir Archivo: %s", pcb_obtener_pid(pcb), nombreArchivoNuevo);
            
                    hayQueReplanificar = false;
                    break;
                }
                
                // El archivo está en la tabla, está abierto, se bloquea el proceso en la cola de bloqueados del archivo
                else if(strcmp(t_archivo_tabla_obtener_nombre_archivo(tablaDeArchivoBuscado),nombreArchivoNuevo) == 0){
                    t_archivo_tabla_agregar_proceso_a_cola_de_bloqueados(tablaDeArchivoBuscado, pcb);
                    pcb_setear_estado(pcb, BLOCKED);
                    loggear_cambio_estado("EXEC", "BLOCKED", pcb_obtener_pid(pcb));
                    log_info(kernelLogger, "PID: %d - Bloqueado por: %s ", pcb_obtener_pid(pcb), nombreArchivoNuevo);
                    free(nombreArchivoNuevo);
                    hayQueReplanificar = true;
                    break;
                }
            case HEADER_proceso_F_CLOSE:
                t_buffer* bufferFCLOSE =buffer_crear();
                cpuRespuesta = stream_recibir_header(kernel_config_obtener_socket_cpu(kernelConfig));
                if(cpuRespuesta!= HEADER_proceso_parametros){
                    log_error(kernelLogger, "error al recibir nombre de archivo");
                    exit(-1);
                }
                stream_recibir_buffer(kernel_config_obtener_socket_cpu(kernelConfig), bufferFCLOSE);
                char* nombreDeArchFClose = malloc(sizeof(*nombreDeArchFClose));
                buffer_desempaquetar_string(bufferFCLOSE, &nombreDeArchFClose);
                t_archivo_tabla* entradaDeTabla = encontrarEntradaEnTablaGlobal(nombreDeArchFClose);
                if(list_is_empty(t_archivo_tabla_obtener_cola_procesos(entradaDeTabla))){
                    t_buffer* buffer_FCLOSE_FS = buffer_crear();
                    buffer_empaquetar_string(buffer_FCLOSE_FS, nombreDeArchFClose);
                    stream_enviar_buffer(kernel_config_obtener_socket_filesystem_peticiones(kernelConfig), HEADER_F_CLOSE, buffer_FCLOSE_FS);
                    buffer_destruir(buffer_FCLOSE_FS);
                }
                else{
                    t_pcb* pcbQueAgarraArchivo = list_remove(t_archivo_tabla_obtener_cola_procesos(entradaDeTabla), 0);
                    t_archivo_tabla_setear_pid(entradaDeTabla, pcb_obtener_pid(pcbQueAgarraArchivo));
                    pcb_setear_estado(pcbQueAgarraArchivo, READY);
                    estado_encolar_pcb_con_semaforo(estadoReady, pcbQueAgarraArchivo);
                    loggear_cambio_estado("BLOCKED", "READY", pcb_obtener_pid(pcbQueAgarraArchivo));
                    pcb_setear_tiempoDellegadaAReady(pcbQueAgarraArchivo);
                    obtenerListaDePids(estadoReady);
                    sem_post(estado_obtener_sem(estadoReady));
                }
                free(nombreDeArchFClose);
                buffer_destruir(bufferFCLOSE);
                hayQueReplanificar = false;
                break;
            case HEADER_proceso_F_READ:
                t_buffer* bufferFREAD =buffer_crear();
                cpuRespuesta = stream_recibir_header(kernel_config_obtener_socket_cpu(kernelConfig));
                if(cpuRespuesta!= HEADER_proceso_parametros){
                    log_error(kernelLogger, "error al recibir nombre de archivo");
                    exit(-1);
                }
                stream_recibir_buffer(kernel_config_obtener_socket_cpu(kernelConfig), bufferFREAD);
                char* nombreDeArchFRead = malloc(sizeof(*nombreDeArchFRead));
                char* cantBytes =malloc(sizeof(*cantBytes));
                uint32_t pidParaFREAD, nroSegmentoFREAD, offsetFREAD; 
                buffer_desempaquetar_string(bufferFREAD, &nombreDeArchFRead);
                buffer_desempaquetar_string(bufferFREAD, &cantBytes);
                buffer_desempaquetar(bufferFREAD, &pidParaFREAD, sizeof(pidParaFREAD));
                buffer_desempaquetar(bufferFREAD, &nroSegmentoFREAD, sizeof(nroSegmentoFREAD));
                buffer_desempaquetar(bufferFREAD, &offsetFREAD, sizeof(offsetFREAD));
                buffer_destruir(bufferFREAD);

                t_buffer* bufferParaMANDARaFS = buffer_crear();
                t_archivo_tabla_proceso* entradaDeTablaDeProcesoFREAD = encontrarArchivoTablaProcesos(nombreDeArchFRead, pcb);
                uint32_t punteroFRead = t_archivo_tabla_proceso_obtener_puntero(entradaDeTablaDeProcesoFREAD);
                buffer_empaquetar_string(bufferParaMANDARaFS, nombreDeArchFRead);
                buffer_empaquetar(bufferParaMANDARaFS, &punteroFRead, sizeof(punteroFRead));
                buffer_empaquetar_string(bufferParaMANDARaFS, cantBytes);
                // para fs

                buffer_empaquetar(bufferParaMANDARaFS, &pidParaFREAD, sizeof(pidParaFREAD));
                buffer_empaquetar(bufferParaMANDARaFS, &nroSegmentoFREAD, sizeof(nroSegmentoFREAD));
                buffer_empaquetar(bufferParaMANDARaFS, &offsetFREAD, sizeof(offsetFREAD));
                // para fs que se lo envia a memoria 

                stream_enviar_buffer(kernel_config_obtener_socket_filesystem_peticiones(kernelConfig),HEADER_F_READ, bufferParaMANDARaFS);
                buffer_destruir(bufferParaMANDARaFS);
                
                pcb_setear_estado(pcb, BLOCKED);
                loggear_cambio_estado("EXEC", "BLOCKED", pcb_obtener_pid(pcb));
                log_info(kernelLogger, "PID: %d - Bloqueado por: %s ", pcb_obtener_pid(pcb), nombreDeArchFRead);
                    
                free(nombreDeArchFRead);
                free(cantBytes);
                /// logica para bloquearlo 
                hayQueReplanificar = false;
                break;
            case HEADER_proceso_F_WRITE:
                t_buffer* bufferWRITE =buffer_crear();
                cpuRespuesta = stream_recibir_header(kernel_config_obtener_socket_cpu(kernelConfig));
                if(cpuRespuesta!= HEADER_proceso_parametros){
                    log_error(kernelLogger, "error al recibir nombre de archivo");
                    exit(-1);
                }
                stream_recibir_buffer(kernel_config_obtener_socket_cpu(kernelConfig), bufferWRITE);
                char* nombreDeArchFWRITE = malloc(sizeof(*nombreDeArchFWRITE));
                char* cantBytesFWRITE =malloc(sizeof(*cantBytesFWRITE));
                uint32_t pidParaFWRITE, nroSegmentoFWRITE, offsetFWRITE, direccionLogica;
                buffer_desempaquetar(bufferWRITE, &direccionLogica, sizeof(direccionLogica));
                buffer_desempaquetar_string(bufferWRITE, &nombreDeArchFWRITE);
                buffer_desempaquetar_string(bufferWRITE, &cantBytesFWRITE);
                buffer_desempaquetar(bufferWRITE, &pidParaFWRITE, sizeof(pidParaFWRITE));
                buffer_desempaquetar(bufferWRITE, &nroSegmentoFWRITE, sizeof(nroSegmentoFWRITE));
                buffer_desempaquetar(bufferWRITE, &offsetFWRITE, sizeof(offsetFWRITE));
                buffer_destruir(bufferWRITE);

                t_buffer* bufferParaMANDARFWRITE = buffer_crear();
                t_archivo_tabla_proceso* entradaDeTablaDeProcesoFWrite = encontrarArchivoTablaProcesos(nombreDeArchFWRITE, pcb);
                uint32_t punteroFWrite = t_archivo_tabla_proceso_obtener_puntero(entradaDeTablaDeProcesoFWrite);
                // para fs
                buffer_empaquetar_string(bufferParaMANDARFWRITE, nombreDeArchFWRITE);
                buffer_empaquetar(bufferParaMANDARFWRITE, &punteroFWrite, sizeof(punteroFWrite));
                buffer_empaquetar_string(bufferParaMANDARFWRITE, cantBytes);
                buffer_empaquetar(bufferParaMANDARFWRITE, &direccionLogica, sizeof(direccionLogica));
                buffer_empaquetar(bufferParaMANDARFWRITE, &pidParaFWRITE, sizeof(pidParaFWRITE));
                buffer_empaquetar(bufferParaMANDARFWRITE, &nroSegmentoFWRITE, sizeof(nroSegmentoFWRITE));
                buffer_empaquetar(bufferParaMANDARFWRITE, &offsetFWRITE, sizeof(offsetFWRITE));
                stream_enviar_buffer(kernel_config_obtener_socket_filesystem_peticiones(kernelConfig),HEADER_F_WRITE, bufferParaMANDARFWRITE);
                buffer_destruir(bufferParaMANDARFWRITE);

                pcb_setear_estado(pcb, BLOCKED);
                loggear_cambio_estado("EXEC", "BLOCKED", pcb_obtener_pid(pcb));
                log_info(kernelLogger, "PID: %d - Bloqueado por: %s ", pcb_obtener_pid(pcb), nombreDeArchFRead);
                
                free(nombreDeArchFWRITE);
                free(cantBytesFWRITE);
                hayQueReplanificar = false;
                break;
            case HEADER_proceso_F_SEEK:
                t_buffer* buffer_F_SEEK = buffer_crear();
                stream_recibir_header(kernel_config_obtener_socket_cpu(kernelConfig));
                stream_recibir_buffer(kernel_config_obtener_socket_cpu(kernelConfig), buffer_F_SEEK);

                char* nombreArchivoFSEEK = malloc(sizeof(*nombreArchivoFSEEK));
                uint32_t punteroFSeek;
                buffer_desempaquetar(buffer_F_SEEK, &punteroFSeek, sizeof(punteroFSeek));
                buffer_desempaquetar_string(buffer_F_SEEK, &nombreArchivoFSEEK );
                
                t_archivo_tabla_proceso* tabladeArchivosDelProceso = encontrarArchivoTablaProcesos(nombreArchivoFSEEK , pcb);
                t_archivo_tabla_proceso_setear_puntero(tabladeArchivosDelProceso,punteroFSeek);
                log_info(kernelLogger, "PID: %d - Actualizar puntero Archivo: %s - Puntero: %d", pcb_obtener_pid(pcb), nombreArchivoFSEEK , punteroFSeek);
                
                buffer_destruir(buffer_F_SEEK);
                free(nombreArchivoFSEEK);
                hayQueReplanificar = false;
                break;
            case HEADER_proceso_F_TRUNCATE:
                t_buffer* bufferF_TRUNCATE = buffer_crear();
                cpuRespuesta = stream_recibir_header(kernel_config_obtener_socket_cpu(kernelConfig));
                if(cpuRespuesta!= HEADER_proceso_parametros){
                    log_info(kernelLogger, "no recibio el mensaje de cpu");
                    exit(-1);
                }
                stream_recibir_buffer(kernel_config_obtener_socket_cpu(kernelConfig), bufferF_TRUNCATE);
                stream_enviar_buffer(kernel_config_obtener_socket_filesystem_peticiones(kernelConfig), HEADER_F_TRUNCATE, bufferF_TRUNCATE);
                
                char* nombreArchivo_F_TRUNCATE = malloc(sizeof(nombreArchivo_F_TRUNCATE));
                char* tamanio_F_TRUNCATE = malloc(sizeof(tamanio_F_TRUNCATE));
                buffer_desempaquetar_string(bufferF_TRUNCATE, &nombreArchivo_F_TRUNCATE);
                buffer_desempaquetar_string(bufferF_TRUNCATE, &tamanio_F_TRUNCATE);                

                buffer_destruir(bufferF_TRUNCATE);
                
                pcb_setear_estado(pcb, BLOCKED);
                estado_encolar_pcb_con_semaforo(estadoBlocked, pcb);
                loggear_cambio_estado("EXEC", "BLOCKED", pcb_obtener_pid(pcb));
                log_info(kernelLogger, "Truncar Archivo - PID: %d - Archivo: %s - Tamanio: %s", pcb_obtener_pid(pcb), nombreArchivo_F_TRUNCATE, tamanio_F_TRUNCATE);
                //  sem_post(estado_obtener_sem(estadoBlocked));
                hayQueReplanificar = true;
                // hacer hilo
                /* Logica para desbloqueo del proceso por un F_TRUNCATE*/
                /* uint8_t respuestaFileSystem = stream_recibir_header(kernel_config_obtener_socket_filesystem_peticiones(kernelConfig));

                if(respuestaFileSystem == HEADER_ERROR_F_TRUNCATE){
                    log_error(kernelLogger, "Error al ejecutar F_TRUNCATE");
                }
                else if(respuestaFileSystem == HEADER_F_TRUNCATE_REALIZADO){
                    pcb_setear_estado(pcb, READY);
                    estado_encolar_pcb_con_semaforo(estadoReady, pcb);
                    loggear_cambio_estado("BLOCKED", "READY", pcb_obtener_pid(pcb));
                    log_info(kernelLogger, "PID: %d - Archivo: %s - Tamaño: %d", pcb_obtener_pid(pcb), nombreArchivo, tamanio);
                }*/
                break;
            case HEADER_create_segment:
                log_info(kernelLogger, "SE LLEGO A CREATE");
                t_buffer* bufferCreateSegment = buffer_crear();
                uint8_t headerCPU = stream_recibir_header(kernel_config_obtener_socket_cpu(kernelConfig));
                stream_recibir_buffer(kernel_config_obtener_socket_cpu(kernelConfig),bufferCreateSegment);
                log_info(kernelLogger, "llego a recibir header  %d", headerCPU);
                uint32_t idCreate;
                uint32_t tamanio;
                buffer_desempaquetar(bufferCreateSegment, &idCreate, sizeof(idCreate));
                buffer_desempaquetar(bufferCreateSegment, &tamanio, sizeof(tamanio));
                buffer_destruir(bufferCreateSegment);

                t_buffer* bufferCreateSegmentParaMemoria = buffer_crear();
                buffer_empaquetar(bufferCreateSegmentParaMemoria, &pidAEnviar, sizeof(pidAEnviar));
                buffer_empaquetar(bufferCreateSegmentParaMemoria, &idCreate, sizeof(idCreate));
                buffer_empaquetar(bufferCreateSegmentParaMemoria, &tamanio, sizeof(tamanio));
                stream_enviar_buffer(socketMemoria, HEADER_create_segment, bufferCreateSegmentParaMemoria);
                buffer_destruir(bufferCreateSegmentParaMemoria);
                
                uint8_t respuestaMemoria = stream_recibir_header(socketMemoria);
                log_info(kernelLogger, "resp de memoria %d", respuestaMemoria);
                if(respuestaMemoria == HEADER_segmento_creado ){
                    t_buffer* bufferSegCreado = buffer_crear();
                    stream_recibir_buffer(socketMemoria,bufferSegCreado);
                    buffer_desempaquetar_tabla_de_segmentos(bufferSegCreado,pcb_obtener_tabla_de_segmentos(pcb),cantidadDeSegmentos);
                    buffer_destruir(bufferSegCreado);
                    hayQueReplanificar = false;
                }else if(respuestaMemoria == HEADER_hay_que_compactar){
                    stream_recibir_buffer_vacio(socketMemoria);
  
                    stream_enviar_buffer_vacio(kernel_config_obtener_socket_compactacion(kernelConfig), HEADER_comprobar_si_hay_operaciones_activas_fs_mem);
                    int rstaFSCOMPACTACION = stream_recibir_header(kernel_config_obtener_socket_compactacion(kernelConfig));
                    if(rstaFSCOMPACTACION != HEADER_OK_puede_continuar){
                        log_error(kernelLogger, "no se recibio correctamente el rstaFSCOMPACTACION se recibio %d", rstaFSCOMPACTACION);
                        exit(-1);
                    }
                    stream_recibir_buffer_vacio(kernel_config_obtener_socket_compactacion(kernelConfig))                    
                    stream_enviar_buffer_vacio(socketMemoria, HEADER_bueno_compacta);

                    respuestaMemoria = stream_recibir_header(socketMemoria);
                    if(respuestaMemoria != HEADER_lista_de_tablas_de_segmentos){
                        log_error(kernelLogger, "NO recbibi lista de tablas de segmentos, recibi: %d", respuestaMemoria);
                        exit(-1);
                    }
                    t_buffer* bufferProcesos = buffer_crear();
                    stream_recibir_buffer(socketMemoria, bufferProcesos);

                    buffer_desempaquetar_y_actualizar_lista_procesos(bufferProcesos);
                    
                    buffer_destruir(bufferProcesos);

                    respuestaMemoria = stream_recibir_header(socketMemoria);
                    if(respuestaMemoria != HEADER_segmento_creado) {
                        log_error(kernelLogger, "Error al recibir header segmento creado, header recibido: %d", respuestaMemoria);
                        exit(-1);
                    }
                    t_buffer* bufferTablaDespuesDeCompactar = buffer_crear();
                    stream_recibir_buffer(socketMemoria, bufferTablaDespuesDeCompactar);
                    buffer_desempaquetar_tabla_de_segmentos(bufferTablaDespuesDeCompactar, pcb_obtener_tabla_de_segmentos(pcb), cantidadDeSegmentos);
                    buffer_destruir(bufferTablaDespuesDeCompactar);
                    hayQueReplanificar = false;
                }else if(respuestaMemoria == HEADER_proceso_terminado_out_of_memory){
                    finalizar_proceso(pcb,OUT_OF_MEMORY);
                    stream_recibir_buffer_vacio(socketMemoria);
                    hayQueReplanificar = true; 
                }else{
                    log_error(kernelLogger, "error al recibir respuesta de memoria para ejecutar create segment" );
                }
                break;
            case HEADER_delete_segment:
                t_buffer *bufferDeleteSegment = buffer_crear();
                stream_recibir_header(kernel_config_obtener_socket_cpu(kernelConfig));
                stream_recibir_buffer(kernel_config_obtener_socket_cpu(kernelConfig), bufferDeleteSegment);

                uint32_t idDelete;
                buffer_desempaquetar(bufferDeleteSegment, &idDelete, sizeof(idDelete));
                buffer_destruir(bufferDeleteSegment);

                t_buffer* bufferDeleteSegmentParaMemoria = buffer_crear();
                buffer_empaquetar(bufferDeleteSegmentParaMemoria, &pidAEnviar, sizeof(pidAEnviar));
                buffer_empaquetar(bufferDeleteSegmentParaMemoria, &idDelete, sizeof(idDelete));
                stream_enviar_buffer(kernel_config_obtener_socket_memoria(kernelConfig), HEADER_delete_segment, bufferDeleteSegmentParaMemoria);
                buffer_destruir(bufferDeleteSegmentParaMemoria);

                hayQueReplanificar = false;
                break;
            default:
                log_error(kernelLogger, "Error al recibir mensaje de CPU");
                exit(-1);
                break;
        }
        if(hayQueReplanificar == true){
            if(algoritmoConfigurado == ALGORITMO_HRRN){
                actualizar_pcb_por_fin_de_rafaga(pcb);
            }
        }
        sem_post(&dispatchPermitido);
    }
}
            


void planificador_corto_plazo() {
    pthread_t atenderPCBHilo;

    pthread_create(&atenderPCBHilo, NULL, (void*)atender_pcb, NULL);
    pthread_detach(atenderPCBHilo);

    for (;;) {
        t_pcb* pcbToDispatch;
            
        sem_wait(&dispatchPermitido); // fijarse si la cpu esta libre
        log_info(kernelLogger, "Se permite dispatch");

        if(hayQueReplanificar){
            sem_wait(estado_obtener_sem(estadoReady));

            log_info(kernelLogger, "Se toma una instancia de READY");

            if(algoritmoConfigurado == ALGORITMO_FIFO){
        
                pcbToDispatch = iniciar_fifo(estadoReady);

            }else{
            
                pcbToDispatch = iniciar_HRRN(estadoReady, kernel_config_obtener_hrrn_alfa(kernelConfig));

            }
        }
        else{
            log_info(kernelLogger, "Se vuelve a ejecutar el ultimo proceso");
            pcbToDispatch = ultimoPcbEjecutado;
        }

        estado_encolar_pcb_con_semaforo(estadoExec, pcbToDispatch);
        sem_post(estado_obtener_sem(estadoExec));
    }
}

void avisar_a_memoria_de_crear_segmentos_de_proceso(t_pcb* pcb){

    t_buffer* buffer = buffer_crear();
    int pid = pcb_obtener_pid(pcb);
    buffer_empaquetar(buffer, &pid, sizeof(pid));
    stream_enviar_buffer(socketMEMORIA,HEADER_proceso_a_agregar_a_memoria,buffer); 
    buffer_destruir(buffer);
}


void* encolar_en_new_nuevo_pcb_entrante(void* socket) {
    int* socketProceso = (int*)socket;
    uint32_t tamanio = 0;

    uint8_t respuestaDeConsola = stream_recibir_header(*socketProceso);
    if (respuestaDeConsola == HANDSHAKE_consola) {
        log_info(kernelLogger, "Se conecto una consola");
        t_buffer* bufferHandshakeInicial = buffer_crear();
        stream_recibir_buffer(*socketProceso, bufferHandshakeInicial);
        buffer_desempaquetar(bufferHandshakeInicial, &tamanio, sizeof(tamanio));
        buffer_destruir(bufferHandshakeInicial);
        stream_enviar_buffer_vacio(*socketProceso, HANDSHAKE_puede_continuar);

        uint8_t respuestaDeConsola2 = stream_recibir_header(*socketProceso);
        if (respuestaDeConsola2 != HEADER_lista_de_instrucciones) {
            log_error(kernelLogger, "Error al intentar recibir lista de instrucciones del proceso mediante <socket %d>", *socketProceso);
            return NULL;
        }

        t_buffer* bufferDeInstrucciones = buffer_crear();
        stream_recibir_buffer(*socketProceso, bufferDeInstrucciones);
        t_buffer* bufferDeInstruccionesCopia = buffer_crear_copia(bufferDeInstrucciones);

        uint32_t nuevoPID = obtener_siguiente_pid();
        t_pcb* nuevoPCB = pcb_crear(nuevoPID, tamanio, kernel_config_obtener_estimacion_inicial(kernelConfig));

        pcb_setear_socket(nuevoPCB, socketProceso);
        pcb_setear_buffer_de_instrucciones(nuevoPCB, bufferDeInstruccionesCopia);

        log_info(kernelLogger, "Creación de nuevo proceso ID %d de tamaño %d mediante <socket %d>", pcb_obtener_pid(nuevoPCB), tamanio, *socketProceso);

        t_buffer* bufferPID = buffer_crear();
        buffer_empaquetar(bufferPID, &nuevoPID, sizeof(nuevoPID));
        stream_enviar_buffer(*socketProceso, HEADER_pid, bufferPID);
        buffer_destruir(bufferPID);

        estado_encolar_pcb_con_semaforo(estadoNew, nuevoPCB);
        loggear_cambio_estado("NULL", "NEW", pcb_obtener_pid(nuevoPCB));
        sem_post(&hayPcbsParaAgregarAlSistema);
        buffer_destruir(bufferDeInstrucciones);
    } else {
        log_error(kernelLogger, "Error al intentar establecer conexión con proceso mediante <socket %d>", *socketProceso);
    }
    return NULL;
}



void iniciar_planificadores(void){
    
    estadoNew = estado_crear(NEW);
    estadoReady = estado_crear(READY);
    estadoExec = estado_crear(EXEC);
    estadoExit = estado_crear(EXIT);
    estadoBlocked = estado_crear(BLOCKED);
    pcbsEsperandoParaIO = estado_crear(PCBS_ESPERANDO_PARA_IO);


    arrayDeRecursos = kernel_config_obtener_recursos(kernelConfig);
    dimensionDeArrayDeRecursos = obtenerDimensionDeArrayDeRecursos(arrayDeRecursos);
    vectorDeInstancias = convertirInstanciasDeRecursoEnEnteros(kernel_config_obtener_instancias_recursos(kernelConfig), dimensionDeArrayDeRecursos);
    tablaArchivosAbiertos = list_create();
    listaDePcbs = list_create();

    pteroAVectorDeListaDeRecursos = malloc(sizeof(*pteroAVectorDeListaDeRecursos)*dimensionDeArrayDeRecursos);
    
    for(int i=0; i<dimensionDeArrayDeRecursos; i++){
        pteroAVectorDeListaDeRecursos[i] = list_create();
    }


    pthread_t largoPlazoHilo;
    pthread_t cortoPlazoHilo;
    pthread_t atenderBloqueoDeFilesystem;
    siguientePID = 1;
    sem_init(&dispatchPermitido,0,1);
    sem_init(&hayPcbsParaAgregarAlSistema,0,0);
    sem_init(&gradoDeMultiprogramacion, 0, kernel_config_obtener_grado_multiprogramacion(kernelConfig));

    pthread_mutex_init(&controlListaPcbs, NULL);
    pthread_mutex_init(&siguientePIDmutex, NULL);

    if (kernel_config_es_algoritmo_hrrn(kernelConfig)) {        
        algoritmoConfigurado = ALGORITMO_HRRN;
        log_info(kernelLogger, "Se crean los hilos planificadores con HRRN");
    } else if (kernel_config_es_algoritmo_fifo(kernelConfig)) {
        algoritmoConfigurado = ALGORITMO_FIFO;
        log_info(kernelLogger, "Se crean los hilos planificadores con FIFO");
    } else {
        log_error(kernelLogger, "error al iniciar planificador. algoritmo no valido");
        exit(-1);
    }

    pthread_create(&largoPlazoHilo, NULL, (void*)planificador_largo_plazo, NULL);
    pthread_detach(largoPlazoHilo);
    pthread_create(&cortoPlazoHilo, NULL, (void*)planificador_corto_plazo, NULL); 
    pthread_detach(cortoPlazoHilo);
    pthread_create(&atenderBloqueoDeFilesystem, NULL, (void*) atenderBloqueoDe_Filesystem, NULL);
    pthread_detach(atenderBloqueoDeFilesystem);

}