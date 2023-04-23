#include "../include/pcb.h"

struct t_pcb {
    uint32_t pid;
    t_buffer* instrucciones;
    uint32_t programCounter;
    time_t tiempoDellegadaAReady;
    // registros de uso gral de cpu (??? )
    // tabla de segmentos
    double estimacionProximaRafaga;
    uint8_t estado;
    // tabla de archivos abiertos con LA info de la POSICION del puntero en cada uno (struct con puntero indicando posicion)

};


uint32_t pcb_obtener_pid(t_pcb* self) {
    return self->pid;
}

t_buffer* pcb_obtener_buffer_de_instrucciones(t_pcb* self) {
    return self->instructionsBuffer;
}
void pcb_setear_buffer_de_instrucciones(t_pcb* self, t_buffer* instructionsBuffer) {
    self->instructionsBuffer = instructionsBuffer;
}

uint32_t pcb_obtener_program_counter(t_pcb* self) {
    return self->programCounter;
}

void pcb_setear_program_counter(t_pcb* self, uint32_t programCounter) {
    self->programCounter = programCounter;
}

time_t pcb_obtener_tiempoDellegadaAReady(t_pcb* self){
    return self->tiempoDellegadaAReady;
}

double pcb_obtener_estimacion_prox_rafaga(t_pcb* self) {
    return self->estimacionProximaRafaga;
}

void pcb_setear_estimado_prox_rafaga(t_pcb* self, double estimacionActual) {
    self->estimacionProximaRafaga = estimacionActual;
}

uint8_t pcb_obtener_estimado_prox_rafaga(t_pcb* self) {
    return self->estado;
}

void pcb_setear_estado_actual(t_pcb* self, uint8_t estadoNuevo) {
    self->estado = estadoNuevo;
}
