#include "../include/planificador.h"
/*

static t_maneja_dispatch elegir_pcb;
static t_maneja_bloqueados actualizar_pcb_por_bloqueo;
static t_maneja_prioridad evaluar_desalojo;

void iniciar_planificadores(void){
        if (kernel_config_es_algoritmo_hrrn(kernelConfig)) {
        elegir_pcb = elegir_pcb_segun_hrrn;
        evaluar_desalojo = evaluar_desalojo_segun_hrrn;
        actualizar_pcb_por_bloqueo = actualizar_pcb_por_bloqueo_segun_hrrn;
    } else if (kernel_config_es_algoritmo_fifo(kernelConfig)) {
        elegir_pcb = elegir_pcb_segun_fifo;
        evaluar_desalojo = evaluar_desalojo_segun_fifo;
        actualizar_pcb_por_bloqueo = actualizar_pcb_por_bloqueo_segun_fifo;
    } else {
        log_error(kernelLogger, "No se pudo inicializar el planificador, no se encontró un algoritmo de planificación válido");
        exit(-1);
    }
}*/