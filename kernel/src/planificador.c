#include "../include/planificador.h"


extern t_log* kernelLogger;
extern t_kernel_config* kernelConfig;

static int algoritmoConfigurado;

extern int socketMEMORIA;

uint32_t siguientePID;

//faltan semaforos
sem_t gradoDeMultiprogramacion;
sem_t hayPcbsParaAgregarAlSistema;
sem_t dispatchPermitido;

struct tm* tiempo1970;
time_t tiempoLocalActual;


pthread_mutex_t siguientePIDmutex;

char** arrayDeRecursos;
int* vectorDeInstancias;
int dimensionDeArrayDeRecursos;
bool hayQueReplanificar = true;
t_pcb* ultimoPcbEjecutado;

t_list** pteroAVectorDeListaDeRecursos;

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
        log_info(kernelLogger, "Se finaliza PCB <ID %d> de tamaño %d por motivo: SUCCESS", pcb_obtener_pid(pcbALiberar), pcb_obtener_tamanio(pcbALiberar));
        break;
    case SEG_FAULT: // caso seg fault
        log_error(kernelLogger, "Se finaliza PCB <ID %d> de tamaño %d por motivo: SEG_FAULT", pcb_obtener_pid(pcbALiberar), pcb_obtener_tamanio(pcbALiberar));
        break;
    case OUT_OF_MEMORY: 
        log_error(kernelLogger, "Se finaliza PCB <ID %d> de tamaño %d por motivo: OUT_OF_MEMORY", pcb_obtener_pid(pcbALiberar), pcb_obtener_tamanio(pcbALiberar));
        break;
    case WAIT_DE_RECURSO_NO_EXISTENTE:
        log_error(kernelLogger, "Se finaliza PCB <ID %d> de tamaño %d por motivo: WAIT_DE_RECURSO_NO_EXISTENTE", pcb_obtener_pid(pcbALiberar), pcb_obtener_tamanio(pcbALiberar));
        break;
    case SIGNAL_DE_RECURSO_NO_EXISTENTE:
        log_error(kernelLogger, "Se finaliza PCB <ID %d> de tamaño %d por motivo: WAIT_DE_RECURSO_NO_EXISTENTE", pcb_obtener_pid(pcbALiberar), pcb_obtener_tamanio(pcbALiberar));
        break;
    }
}

void finalizar_pcbs_en_hilo_con_exit(void) {
    for (;;) {
        sem_wait(estado_obtener_sem(estadoExit));
        t_pcb* pcbALiberar = estado_desencolar_primer_pcb_con_semaforo(estadoExit);
     // avisar a memoria que finalizo   mem_adapter_finalizar_proceso(pcbALiberar, kernelConfig, kernelLogger);
    // ESTO YA ESTA EN FINALIZA    log_info(kernelLogger, "Se finaliza PCB <ID %d> de tamaño %d", pcb_obtener_pid(pcbALiberar), pcb_obtener_tamanio(pcbALiberar));
        stream_enviar_buffer_vacio(pcb_obtener_socket_consola(pcbALiberar), HEADER_proceso_terminado);
        pcb_destruir(pcbALiberar);
        sem_post(&gradoDeMultiprogramacion);
    }
}

/*                          INICIO DISPOSITIVO I/O                       */

void iniciar_io(t_pcb* pcb) {
   
    log_info(kernelLogger, "Ejecutando ráfagas I/O de PCB <ID %d> por %u milisegundos", pcb_obtener_pid(pcb), pcb_obtener_tiempo_bloqueo(pcb));
      
    intervalo_de_pausa(pcb_obtener_tiempo_bloqueo(pcb));

    pcb_setear_estado(pcb, READY);
    estado_encolar_pcb_con_semaforo(estadoReady, pcb);
    pcb_setear_tiempo_bloqueo(pcb, 0);
    
    loggear_cambio_estado("BLOCKED", "READY", pcb_obtener_pid(pcb));
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
        // controlar con memoria el espacio (ver tp)
            estado_encolar_pcb_con_semaforo(estadoReady, pcbQuePasaAReady);
            loggear_cambio_estado("NEW", "READY", pcb_obtener_pid(pcbQuePasaAReady));
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
                if(algoritmoConfigurado == ALGORITMO_HRRN){
                    actualizar_pcb_por_fin_de_rafaga(pcb);
                }
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

}


void atender_signal(char* recurso, t_pcb* pcb){
    
    char** pteroARecursos = kernel_config_obtener_recursos(kernelConfig);
    int i;
    for(i=0; i<dimensionDeArrayDeRecursos;i++){

        if(strcmp(*pteroARecursos, recurso) == 0){
            vectorDeInstancias[i]++;
            if(vectorDeInstancias[i] >= 0){
                t_pcb* pcbADesbloquear = list_get(pteroAVectorDeListaDeRecursos[i], 0);  //agarra el primero de la cola de bloqueados del recurso
                list_remove(pteroAVectorDeListaDeRecursos[i],0);
                pcb_setear_estado(pcbADesbloquear, READY);
                estado_encolar_pcb_con_semaforo(estadoReady, pcbADesbloquear);
                loggear_cambio_estado("BLOCKED", "READY", pcb_obtener_pid(pcbADesbloquear));
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

}


void atender_pcb() {
    for (;;) {
        sem_wait(estado_obtener_sem(estadoExec)); 
        
        pthread_mutex_lock(estado_obtener_mutex(estadoExec));
        
        t_pcb* pcb = list_get(estado_obtener_lista(estadoExec), 0); // saca el primer pcb de la lista
        pthread_mutex_unlock(estado_obtener_mutex(estadoExec));
        
        loggear_cambio_estado("READY", "EXEC", pcb_obtener_pid(pcb));
        // tiempo real ejecutado - rafaga
        struct timespec start;  
        __set_timespec(&start);
    
        kernel_enviar_pcb_a_cpu(pcb, kernelConfig, kernelLogger, HEADER_pcb_a_ejecutar);
        uint8_t cpuRespuesta = stream_recibir_header(kernel_config_obtener_socket_cpu(kernelConfig));

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

        switch (cpuRespuesta) {
            case HEADER_proceso_terminado:
                finalizar_proceso(pcb,SUCCESS);
                hayQueReplanificar = true; 
                break;
            case HEADER_proceso_bloqueado:     
                if(algoritmoConfigurado == ALGORITMO_HRRN ){
                    actualizar_pcb_por_fin_de_rafaga(pcb);
                }
                    //EN FIFO NO SE HACE NADA   
                atender_bloqueo(pcb);   // en ambos se atiende el bloqueo
                hayQueReplanificar = true; 
                break;        
            case HEADER_proceso_wait: 
                t_buffer* bufferWAIT = buffer_crear();
                stream_recibir_header(kernel_config_obtener_socket_cpu(kernelConfig));
                stream_recibir_buffer(kernel_config_obtener_socket_cpu(kernelConfig),bufferWAIT);
                char* recursoDesempaquetadoWAIT;
                buffer_desempaquetar_string(bufferWAIT, &recursoDesempaquetadoWAIT);
                atender_wait(recursoDesempaquetadoWAIT,pcb);
                buffer_destruir(bufferWAIT);
                break;
            case HEADER_proceso_signal:
                t_buffer* bufferSIGNAL = buffer_crear();
                stream_recibir_header(kernel_config_obtener_socket_cpu(kernelConfig));
                stream_recibir_buffer(kernel_config_obtener_socket_cpu(kernelConfig),bufferSIGNAL);
                char* recursoDesempaquetadoSIGNAL;
                buffer_desempaquetar_string(bufferSIGNAL, &recursoDesempaquetadoSIGNAL);   
                atender_signal(recursoDesempaquetadoSIGNAL,pcb);
                buffer_destruir(bufferSIGNAL);
                break;
            case HEADER_proceso_yield:
                if(algoritmoConfigurado == ALGORITMO_HRRN){
                    actualizar_pcb_por_fin_de_rafaga(pcb);
                }
                pcb_setear_estado(pcb, READY);
                estado_encolar_pcb_con_semaforo(estadoReady, pcb);
                loggear_cambio_estado("EXEC", "READY", pcb_obtener_pid(pcb));
                pcb_setear_tiempoDellegadaAReady(pcb);
                sem_post(estado_obtener_sem(estadoReady));
                hayQueReplanificar = true; 
                break;
            case HEADER_create_segment:
                t_buffer* bufferCreateSegment = buffer_crear();
                stream_recibir_header(kernel_config_obtener_socket_cpu(kernelConfig));
                stream_recibir_buffer(kernel_config_obtener_socket_cpu(kernelConfig),bufferCreateSegment);
                stream_enviar_buffer(kernel_config_obtener_socket_memoria(kernelConfig), HEADER_create_segment, bufferCreateSegment);
                buffer_destruir(bufferCreateSegment);
                ////
                uint8_t respuestaMemoria = stream_recibir_header(kernel_config_obtener_socket_memoria(kernelConfig));
                stream_recibir_buffer_vacio(kernel_config_obtener_socket_memoria(kernelConfig));
                switch (respuestaMemoria)
                {
                case HEADER_proceso_terminado_out_of_memory:
                    finalizar_proceso(pcb,OUT_OF_MEMORY);
                    break;
    
                default:
                    break;
                }
                
            case HEADER_delete_segment:
                t_buffer *bufferDeleteSegment = buffer_crear();
                stream_recibir_header(kernel_config_obtener_socket_cpu(kernelConfig));
                stream_recibir_buffer(kernel_config_obtener_socket_cpu(kernelConfig), bufferDeleteSegment);
                stream_enviar_buffer(kernel_config_obtener_socket_memoria(kernelConfig), HEADER_delete_segment, bufferDeleteSegment);
                buffer_destruir(bufferDeleteSegment);
            default:
                log_error(kernelLogger, "Error al recibir mensaje de CPU");
                break;
        }
        sem_post(&dispatchPermitido);
    }
}
                // habria que destruir los buffers en cada CASE???


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
        
        avisar_a_memoria_de_crear_segmentos_de_proceso(nuevoPCB);
        

        
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
    vectorDeInstancias = convertirInstanciasDeRecursoEnEnteros(arrayDeRecursos, dimensionDeArrayDeRecursos);


    pteroAVectorDeListaDeRecursos = malloc(sizeof(*pteroAVectorDeListaDeRecursos)*dimensionDeArrayDeRecursos);
    
    for(int i=0; i<dimensionDeArrayDeRecursos; i++){
        pteroAVectorDeListaDeRecursos[i] = list_create();
    }


    pthread_t largoPlazoHilo;
    pthread_t cortoPlazoHilo;
    
    siguientePID = 1;
    sem_init(&dispatchPermitido,0,1);
    sem_init(&hayPcbsParaAgregarAlSistema,0,0);
    sem_init(&gradoDeMultiprogramacion, 0, kernel_config_obtener_grado_multiprogramacion(kernelConfig));

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



}