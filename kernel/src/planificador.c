#include "../include/planificador.h"
#include "../include/estado.h"
#include "kernel_config.c"


extern t_log* kernelLogger;
extern t_kernel_config* kernelConfig;

//faltan semaforos
static sem_t gradoMultiprog;

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
static void noreturn __hilo_que_libera_pcbs_en_exit(void) {
    for (;;) {
        sem_wait(estado_get_sem(estadoExit));
        t_pcb* pcbALiberar = estado_desencolar_primer_pcb_atomic(estadoExit);
        mem_adapter_finalizar_proceso(pcbALiberar, kernelConfig, kernelLogger);
        log_info(kernelLogger, "Se finaliza PCB <ID %d> de tamaño %d", pcb_get_pid(pcbALiberar), pcb_get_tamanio(pcbALiberar));
        stream_enviar_buffer_vacio(pcb_get_socket(pcbALiberar), HANDSHAKE_puede_continuar);
        pcb_destroy(pcbALiberar);
        sem_post(&gradoMultiprog);
    }
}

/*                  PLANIFICADOR A LARGO PLAZO                              */

static void noreturn __planificador_largo_plazo(void) {
    pthread_t liberarPcbsEnExitTh;
    pthread_create(&liberarPcbsEnExitTh, NULL, (void*)__hilo_que_libera_pcbs_en_exit, NULL);
    pthread_detach(liberarPcbsEnExitTh);

    for (;;) {
        sem_wait(&hayPcbsParaAgregarAlSistema);
        sem_wait(&gradoMultiprog);

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
    if (kernel_config_es_algoritmo_hrrn(kernelConfig)) {
        elegir_pcb_segun_hrrn(estado,kernel_config_obtener_hrrn_alfa(kernelConfig));
        //evaluar_desalojo = evaluar_desalojo_segun_hrrn;
        //actualizar_pcb_por_bloqueo = actualizar_pcb_por_bloqueo_segun_hrrn;
    } else if (kernel_config_es_algoritmo_fifo(kernelConfig)) {
        elegir_pcb_segun_fifo(estado);
    } else {
        log_error(kernelLogger, "No se pudo inicializar el planificador, no se encontró un algoritmo de planificación válido");
        exit(-1);
    }


    
}