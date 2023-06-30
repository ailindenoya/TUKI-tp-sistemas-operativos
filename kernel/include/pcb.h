#ifndef PCB_H_INCLUDED
#define PCB_H_INCLUDED


#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include "../../utils/include/buffer.h"
#include "../../utils/include/funcionesDeMemoria.h"
#include <pthread.h>
#include "nombre_estados.h"
#include "tabla_de_archivos_abiertos.h"
#include "../../utils/include/funcionesDeRegistrosDeCpu.h"

typedef struct t_pcb {
    uint32_t pid;
    uint32_t tamanio;
    t_buffer* instrucciones;
    uint32_t programCounter;
    time_t tiempoDellegadaAReady;
   
    double estimacionProximaRafaga; ///EST n+1
    double realAnterior; // TE n real anterior
    uint32_t tiempoDeBloqueo;
    uint32_t tiempoEjecutado;
    
    registros registrosDeCPU;
    segmento* tablaDeSegmentos;

    uint8_t estado;
    t_list* tablaDeArchivosAbiertos;
    int* socketConsola; // para saber a que consola pertenece

}t_pcb;

uint8_t pcb_obtener_estado(t_pcb* self) ;
t_pcb* pcb_crear(uint32_t pid, uint32_t tamanio, double estimacionInicialParaHRRN);
void pcb_destruir(t_pcb* self);
int pcb_obtener_socket_consola(t_pcb* self);
void pcb_setear_socket(t_pcb* self, int* socketConsola);
uint32_t pcb_obtener_tamanio(t_pcb* self);
uint32_t pcb_obtener_pid(t_pcb* self);
t_buffer* pcb_obtener_buffer_de_instrucciones(t_pcb* self);
void pcb_setear_buffer_de_instrucciones(t_pcb* self, t_buffer* instructionsBuffer);
uint32_t pcb_obtener_program_counter(t_pcb* self);
void pcb_setear_program_counter(t_pcb* self, uint32_t programCounter);
time_t pcb_obtener_tiempoDellegadaAReady(t_pcb* self);
void pcb_setear_tiempoDellegadaAReady(t_pcb* self);
double pcb_obtener_estimacion_prox_rafaga(t_pcb* self);
void pcb_setear_estimacion_prox_rafaga(t_pcb* self, double estimacionActual);
void pcb_setear_estado(t_pcb* self, uint8_t estadoNuevo);
void pcb_setear_tiempo_bloqueo(t_pcb* self, uint32_t tiempoDeBloqueo);
uint32_t pcb_obtener_tiempo_bloqueo(t_pcb* self);
double pcb_obtener_realAnterior(t_pcb* self);
void pcb_setear_realAnterior(t_pcb* self, double nuevo);
void pcb_setear_tiempoEjecutado(t_pcb* self, u_int32_t nuevo);
uint32_t pcb_obtener_tiempoEjecutado(t_pcb* self);
segmento* pcb_obtener_tabla_de_segmentos(t_pcb* self);
void pcb_setear_tabla_de_segmentos(t_pcb* self, segmento* tablaDeSegs, int cantidadDeSegs);
void inicializar_tabla_de_segmentos(segmento* tablaDeSegmentos, int cantidadDeSegmentos);
t_list* pcb_obtener_tabla_de_archivos_abiertos(t_pcb* self);
void pcb_agregar_a_tabla_de_archivos_abiertos(t_pcb* self, t_archivo_tabla_proceso* tablaArchivoAbierto);
registros* pcb_obtener_registros(t_pcb* self);

#endif