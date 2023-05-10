#include "../include/planificador.h"


extern t_log* kernelLogger;
extern t_kernel_config* kernelConfig;


uint32_t siguientePID;
 double alfa = kernel_config_obtener_hrrn_alfa(kernelConfig);

//faltan semaforos
sem_t gradoDeMultiprogramacion;
sem_t hayPcbsParaAgregarAlSistema;
sem_t dispatchPermitido;



pthread_mutex_t siguientePIDmutex;


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
double response_ratio(double rafaga){
    return alfa *(1/rafaga);
}

t_pcb* mayor_response_ratio(t_pcb* unPcb, t_pcb* otroPcb){
    double responseRatioDeUno = response_ratio(pcb_obtener_rafaga(unPcb) );
    double responseRatioDeOtro = response_ratio(pcb_obtener_rafaga(otroPcb));

    if(responseRatioDeUno > responseRatioDeOtro){
        return unPcb;
    }
    else{
        return otroPcb;
    }
}

t_pcb* iniciar_HRRN(t_estado* estado, double alfa) {
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

 void finalizar_pcbs_en_hilo_con_exit(void) {
    for (;;) {
        sem_wait(estado_obtener_sem(estadoExit));
        t_pcb* pcbALiberar = estado_desencolar_primer_pcb_atomic(estadoExit);
     // avisar a memoria que finalizo   mem_adapter_finalizar_proceso(pcbALiberar, kernelConfig, kernelLogger);
        log_info(kernelLogger, "Se finaliza PCB <ID %d> de tamaño %d", pcb_obtener_pid(pcbALiberar), pcb_obtener_tamanio(pcbALiberar));
        stream_enviar_buffer_vacio(pcb_obtener_socket_consola(pcbALiberar), HANDSHAKE_puede_continuar);
        pcb_destruir(pcbALiberar);
        sem_post(&gradoDeMultiprogramacion);
    }
}

/*                          INICIO DISPOSITIVO I/O                       */

void iniciar_io(void) {
    for (;;) {
        sem_wait(estado_obtener_sem(pcbsEsperandoParaIO));
        t_pcb* pcbAEjecutarRafagasIO = estado_desencolar_primer_pcb_con_semaforo(pcbsEsperandoParaIO);
        log_info(kernelLogger, "Ejecutando ráfagas I/O de PCB <ID %d> por %d milisegundos", pcb_obtener_pid(pcbAEjecutarRafagasIO), pcb_obtener_tiempo_bloqueo(pcbAEjecutarRafagasIO));
      //  pcb_test_and_set_tiempo_final_bloqueado(pcbAEjecutarRafagasIO); supuestamente para SUSPENDED BLOCKED

        intervalo_de_pausa(pcb_obtener_tiempo_bloqueo(pcbAEjecutarRafagasIO));
        pthread_mutex_lock(pcb_obtener_mutex(pcbAEjecutarRafagasIO));

        if (pcb_obtener_estado(pcbAEjecutarRafagasIO) == BLOCKED) {

            estado_remover_pcb_de_cola_con_semaforo(estadoBlocked, pcbAEjecutarRafagasIO);
            pcb_setear_estado(pcbAEjecutarRafagasIO, READY);
            estado_encolar_pcb_con_semaforo(estadoReady, pcbAEjecutarRafagasIO);
            pcb_setear_tiempo_bloqueo(pcbAEjecutarRafagasIO, 0);
            loggear_cambio_estado("BLOCKED", "READY", pcb_obtener_pid(pcbAEjecutarRafagasIO));
            sem_post(estado_obtener_sem(estadoReady));
        } 
       // pcb_marcar_tiempo_final_como_no_establecido(pcbAEjecutarRafagasIO); supuestamente para SUSPENDED BLOCKED
        pthread_mutex_unlock(pcb_obtener_mutex(pcbAEjecutarRafagasIO));
    }
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



void actualizar_pcb_por_bloqueo_HRRN(pcb, realEjecutado, kernel_config_get_alfa(kernelConfig)){
    //TODO implementar
}

void atender_bloqueo(t_pcb* pcb) {
  //  pcb_marcar_tiempo_inicial_bloqueado(pcb); PREGUNTA
    estado_encolar_pcb_con_semaforo(pcbsEsperandoParaIO, pcb);
    loggear_cambio_estado("EXEC", "BLOCKED", pcb_obtener_pid(pcb));
    log_info(kernelLogger, "PCB <ID %d> ingresa a la cola de espera de I/O", pcb_get_pid(pcb));
    sem_post(estado_obtener_sem(pcbsEsperandoParaIO));
    pcb_setear_estado(pcb, BLOCKED);
    estado_encolar_pcb_con_semaforo(estadoBlocked, pcb);
}

void atender_pcb(int algoritmoUsado) {
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
        
        list_remove(estado_get_list(estadoExec), 0); // saca de ejec el proceso

        pthread_mutex_unlock(estado_get_mutex(estadoExec));

        uint32_t realEjecutado = 0;
        realEjecutado = obtener_tiempo_en_milisegundos(end, start); // obtener_es

        log_debug(kernelLogger, "PCB <ID %d> estuvo en ejecución por %d milisegundos", pcb_obtener_pid(pcb), realEjecutado);       

        switch (cpuRespuesta) {
            case HEADER_proceso_terminado:
                pcb_setear_estado(pcb, EXIT);
                estado_encolar_pcb_con_semaforo(estadoExit, pcb);
                loggear_cambio_estado("EXEC", "EXIT", pcb_obtener_pid(pcb));
                stream_enviar_buffer_vacio(pcb_obtener_socket_consola(pcb), HEADER_proceso_terminado);
                sem_post(estado_obtener_sem(estadoExit));
                break;
            case HEADER_proceso_bloqueado:      //TODO ver caso de utilizacion de recursos
                if(algoritmoUsado == ALGORITMO_HRRN ){
                actualizar_pcb_por_bloqueo_HRRN(pcb, realEjecutado, kernel_config_get_alfa(kernelConfig)) 
                }else{ 
                    //EN FIFO NO SE HACE NADA
                }
                atender_bloqueo(pcb);   // en ambos se atiende el bloqueo
                break;        // VER COMO ATENDER POR I/O (SIN COLAS) Y POR RECURSOS (UNA COLA POR RECURSO)
            case HEADER_proceso_yield:
                pcb_setear_estado(pcb, READY);
                estado_encolar_pcb_con_semaforo(estadoReady, pcb);
                loggear_cambio_estado("EXEC", "READY", pcb_obtener_pid(pcb));
                break;
            default:
                log_error(kernelLogger, "Error al recibir mensaje de CPU");
                break;
        }

        sem_post(&dispatchPermitido);
    }
}


 void planificador_corto_plazo(int tipoDeAlgoritmo) {
    pthread_t atenderPCBHilo;
    pthread_create(&atenderPCBHilo, NULL, (void*)atender_pcb(tipoDeAlgoritmo), NULL);
    pthread_detach(atenderPCBHilo);

    for (;;) {
        sem_wait(estado_obtener_sem(estadoReady));
        log_info(kernelLogger, "Se toma una instancia de READY");
        t_pcb* pcbToDispatch;
        if(tipoDeAlgoritmo == ALGORITMO_FIFO){
       
        pcbToDispatch = iniciar_fifo(estadoReady);

        }else{

            // HRRN
        }

        estado_encolar_pcb_con_semaforo(estadoExec, pcbToDispatch);
        sem_post(estado_obtener_sem(estadoExec));
    }
}





void iniciar_planificadores(void){
    
    estadoNew = estado_create(NEW);
    estadoReady = estado_create(READY);
    estadoExec = estado_create(EXEC);
    estadoExit = estado_create(EXIT);
    estadoBlocked = estado_create(BLOCKED);
    pcbsEsperandoParaIO = estado_create(PCBS_ESPERANDO_PARA_IO);

    pthread_t largoPlazoHilo;
    pthread_t cortoPlazoHilo;
    pthread_t dispositivoIOHilo;

    //pthread_mutex_init(&mutexSocketMemoria, NULL);
    
    siguientePID = 1;

    sem_init(&gradoDeMultiprogramacion, 0, kernel_config_obtener_grado_multiprogramacion(kernelConfig));

    t_algoritmo algoritmoConfigurado; 
    if (kernel_config_es_algoritmo_hrrn(kernelConfig)) {        
        //evaluar_desalojo = evaluar_desalojo_segun_hrrn;
        //actualizar_pcb_por_bloqueo = actualizar_pcb_por_bloqueo_segun_hrrn;
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
    pthread_create(&cortoPlazoHilo, NULL, (void*)planificador_corto_plazo(algoritmoConfigurado), NULL); //No debería pasarse el algoritmo como 4to parametro???
    pthread_detach(cortoPlazoHilo);
    pthread_create(&dispositivoIOHilo, NULL, (void*)iniciar_io, NULL);
    pthread_detach(dispositivoIOHilo);
}
/*
void* encolar_en_new_nuevo_pcb_entrante(void* socket) {
    int* socketProceso = (int*)socket;
    uint32_t tamanio = 0;

    uint8_t response = stream_recibir_header(*socketProceso);
    if (response == HANDSHAKE_consola) {
        t_buffer* bufferHandshakeInicial = buffer_crear();
        stream_recibir_buffer(*socketProceso, bufferHandshakeInicial);
        buffer_desempaquetar(bufferHandshakeInicial, &tamanio, sizeof(tamanio));
        buffer_destruir(bufferHandshakeInicial);
        stream_enviar_buffer_vacio(*socketProceso, HANDSHAKE_ok_continue);

        uint8_t consolaResponse = stream_recibir_header(*socketProceso);
        if (consolaResponse != HEADER_lista_instrucciones) {
            log_error(kernelLogger, "Error al intentar recibir lista de instrucciones del proceso mediante <socket %d>", *socketProceso);
            return NULL;
        }

        t_buffer* instructionsBuffer = buffer_crear();
        stream_recibir_buffer(*socketProceso, instructionsBuffer);
        t_buffer* instructionsBufferCopy = buffer_crear_copia(instructionsBuffer);

        uint32_t newPid = __obtener_siguiente_pid();
        t_pcb* newPcb = pcb_crear(newPid, tamanio, kernel_config_obtener_estimacion_inicial(kernelConfig));
        pcb_setear_socket(newPcb, socketProceso);
        pcb_setear_buffer_de_instrucciones(newPcb, instructionsBufferCopy);

        log_info(kernelLogger, "Creación de nuevo proceso ID %d de tamaño %d mediante <socket %d>", pcb_obtener_pid(newPcb), tamanio, *socketProceso);

        t_buffer* bufferPID = buffer_crear();
        buffer_empaquetar(bufferPID, &newPid, sizeof(newPid));
        stream_enviar_buffer(*socketProceso, HEADER_pid, bufferPID);
        buffer_destruir(bufferPID);

        estado_encolar_pcb_con_semaforo(estadoNew, newPcb);
        __log_transition("NULL", "NEW", pcb_obtener_pid(newPcb));
        sem_post(&hayPcbsParaAgregarAlSistema);
        buffer_destruir(instructionsBuffer);
    } else {
        log_error(kernelLogger, "Error al intentar establecer conexión con proceso mediante <socket %d>", *socketProceso);
    }
    return NULL;
}*/