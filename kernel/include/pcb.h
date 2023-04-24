#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include "../../utils/include/buffer.h"
#include "estado.h"


typedef struct t_pcb t_pcb;

t_pcb* pcb_crear(uint32_t pid, uint32_t tamanio, double estimacion);
void pcb_destruir(t_pcb* self);
int pcb_obtener_socket(t_pcb* self);
void pcb_setear_socket(t_pcb* self, int* socketConsola);
uint32_t pcb_obtener_tamanio(t_pcb* self);
uint32_t pcb_obtener_pid(t_pcb* self);
t_buffer* pcb_obtener_buffer_de_instrucciones(t_pcb* self);
void pcb_setear_buffer_de_instrucciones(t_pcb* self, t_buffer* instructionsBuffer);
uint32_t pcb_obtener_program_counter(t_pcb* self);
void pcb_setear_program_counter(t_pcb* self, uint32_t programCounter);
time_t pcb_obtener_tiempoDellegadaAReady(t_pcb* self);
double pcb_obtener_estimacion_prox_rafaga(t_pcb* self);
void pcb_setear_estimado_prox_rafaga(t_pcb* self, double estimacionActual);
uint8_t pcb_obtener_estimado_prox_rafaga(t_pcb* self);
void pcb_setear_estado_actual(t_pcb* self, uint8_t estadoNuevo);