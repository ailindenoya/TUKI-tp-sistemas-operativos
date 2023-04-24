#include "../include/planificador.h"


extern t_log* kernelLogger;
extern t_kernel_config* kernelConfig;

//faltan semaforos
 sem_t gradoDeMultiprogramacion;
 sem_t hayPcbsParaAgregarAlSistema;

t_estado* estadoBlocked;
t_estado* estadoExec;
t_estado* estadoExit;
t_estado* estadoNew;
t_estado* estadoReady; 
//t_estado* pcbsEsperandoParaIO;


/*                  ALGORITMOS DE PLANIFICACION                              */
/*                            FIFO                                           */

t_pcb* elegir_pcb_segun_fifo(t_estado* estado){
    pthread_mutex_lock(estado_obtener_mutex(estado));
    t_pcb* pcb = estado_desencolar_primer_pcb(estado);
    pthread_mutex_unlock(estado_obtener_mutex(estado));
    return pcb;
}

/*                          FINALIZADOR DE PCBs                        */
 void finalizar_pcbs_en_hilo_con_exit(void) {
    for (;;) {
        sem_wait(estado_obtener_sem(estadoExit));
        t_pcb* pcbALiberar = estado_desencolar_primer_pcb_atomic(estadoExit);
     // avisar a memoria que finalizo   mem_adapter_finalizar_proceso(pcbALiberar, kernelConfig, kernelLogger);
        log_info(kernelLogger, "Se finaliza PCB <ID %d> de tamaño %d", pcb_obtener_pid(pcbALiberar), pcb_obtener_tamanio(pcbALiberar));
        stream_enviar_buffer_vacio(pcb_obtener_socket(pcbALiberar), HANDSHAKE_puede_continuar);
        pcb_destruir(pcbALiberar);
        sem_post(&gradoDeMultiprogramacion);
    }
}

/*                  PLANIFICADOR A LARGO PLAZO                              */

static void  planificador_largo_plazo(void) {
    pthread_t liberarPcbsEnExitHilo;
    pthread_create(&liberarPcbsEnExitHilo, NULL, (void*)finalizar_pcbs_en_hilo_con_exit, NULL);
    pthread_detach(liberarPcbsEnExitHilo);

    for (;;) {
        sem_wait(&hayPcbsParaAgregarAlSistema);
        sem_wait(&gradoDeMultiprogramacion);

        pthread_mutex_lock(estado_get_mutex(estadoNew));
        t_pcb* pcbQuePasaAReady = list_remove(estado_get_list(estadoNew), 0);
        pthread_mutex_unlock(estado_get_mutex(estadoNew));
        // controlar con memoria el espacio (ver tp)
            estado_encolar_pcb_atomic(estadoReady, pcbQuePasaAReady);
            //__log_transition("NEW", "READY", pcb_get_pid(pcbQuePasaAReady));
            sem_post(estado_get_sem(estadoReady));

        pcbQuePasaAReady = NULL;
    }
}


void iniciar_planificadores(void){
    
    estadoNew = estado_create(NEW);
    estadoReady = estado_create(READY);
    estadoExec = estado_create(EXEC);
    estadoExit = estado_create(EXIT);
    estadoBlocked = estado_create(BLOCKED);

if (kernel_config_es_algoritmo_hrrn(kernelConfig)) {
    elegir_pcb_segun_hrrn(estado,kernel_config_obtener_hrrn_alfa(kernelConfig));
        //evaluar_desalojo = evaluar_desalojo_segun_hrrn;
        //actualizar_pcb_por_bloqueo = actualizar_pcb_por_bloqueo_segun_hrrn;

        
    pthread_t largoPlazoHilo;
    pthread_create(&largoPlazoHilo, NULL, (void*)planificador_largo_plazo, NULL);
    pthread_detach(largoPlazoHilo);
    
} else if (kernel_config_es_algoritmo_fifo(kernelConfig)) {
        elegir_pcb_segun_fifo(estado);


    pthread_t largoPlazoHilo;
    pthread_create(&largoPlazoHilo, NULL, (void*)planificador_largo_plazo, NULL);
    pthread_detach(largoPlazoHilo);
} else {
        log_error(kernelLogger, "error al iniciar planificador. algoritmo no valido");
        exit(-1);
}

    
}