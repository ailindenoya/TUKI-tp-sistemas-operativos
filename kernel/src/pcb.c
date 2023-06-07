#include "../include/pcb.h"

struct t_pcb {
    uint32_t pid;
    uint32_t tamanio;
    t_buffer* instrucciones;
    uint32_t programCounter;
    time_t tiempoDellegadaAReady;
    // registros de uso gral de cpu (??? )
    // tabla de segmentos

    double estimacionProximaRafaga; ///EST n+1
    double realAnterior; // TE n real anterior

    uint32_t tiempoEjecutado;

    uint8_t estado;
    double tiempoDeBloqueo;
    // tabla de archivos abiertos con LA info de la POSICION del puntero en cada uno (struct con puntero indicando posicion)
    int* socketConsola; // para saber a que consola pertenece

};

t_pcb* pcb_crear(uint32_t pid, uint32_t tamanio, double estimacionInicialParaHRRN) {
    t_pcb* self = malloc(sizeof(*self));
    self->pid = pid;
    self->tamanio = tamanio;
    self->programCounter = 0;;
    self->estado = NEW;
    self->socketConsola = NULL;
    self->instrucciones = NULL;
    self->estimacionProximaRafaga = estimacionInicialParaHRRN;
    self->realAnterior = 0.0;
    self->tiempoDeBloqueo = 0.0;
    self->tiempoEjecutado = 0.0;
    return self;
}

void pcb_destruir(t_pcb* self) {
    if (self->instrucciones != NULL) {
        buffer_destruir(self->instrucciones);
    }
    if (self->socketConsola != NULL) {
        close(*self->socketConsola);
        free(self->socketConsola);
    }
    pthread_mutex_destroy(self->mutex);
    free(self->mutex);
    free(self);
}
pthread_mutex_t* pcb_obtener_mutex(t_pcb* self) {
    return self->mutex;
}
uint32_t pcb_obtener_tiempo_bloqueo(t_pcb* self){
    return self->tiempoDeBloqueo;
}
void pcb_setear_tiempo_bloqueo(t_pcb* self, uint32_t tiempoDeBloqueo) {
    self->tiempoDeBloqueo = tiempoDeBloqueo;
}
uint32_t pcb_obtener_tamanio(t_pcb* self){
    return self->tamanio;
}
uint32_t pcb_obtener_pid(t_pcb* self) {
    return self->pid;
}
void pcb_setear_socket(t_pcb* self, int* socketConsola) {
    self->socketConsola = socketConsola;
}
int pcb_obtener_socket_consola(t_pcb* self) {
    return *self->socketConsola;
}
t_buffer* pcb_obtener_buffer_de_instrucciones(t_pcb* self) {
    return self->instrucciones;
}
void pcb_setear_buffer_de_instrucciones(t_pcb* self, t_buffer* instructionsBuffer) {
    self->instrucciones = instructionsBuffer;
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
void pcb_setear_tiempoDellegadaAReady(t_pcb* self) {
    time(&self->tiempoDellegadaAReady);
}
double pcb_obtener_estimacion_prox_rafaga(t_pcb* self) {
    return self->estimacionProximaRafaga;
}
void pcb_setear_estimacion_prox_rafaga(t_pcb* self, double estimacionActual) {
    self->estimacionProximaRafaga = estimacionActual;
}
uint8_t pcb_obtener_estimado_prox_rafaga(t_pcb* self) {
    return self->estado;
}
uint8_t pcb_obtener_estado(t_pcb* self) {
    return self->estado;
}
void pcb_setear_estado(t_pcb* self, uint8_t estadoNuevo) {
    self->estado = estadoNuevo;
}
double pcb_obtener_realAnterior(t_pcb* self){
    return self->realAnterior;
}
void pcb_setear_realAnterior(t_pcb* self, double nuevo){
   self->realAnterior = nuevo;
}
void pcb_setear_tiempoEjecutado(t_pcb* self, u_int32_t nuevo){
   self->tiempoEjecutado = nuevo;
}
uint32_t pcb_obtener_tiempoEjecutado(t_pcb* self){
   return self->tiempoEjecutado;
}
