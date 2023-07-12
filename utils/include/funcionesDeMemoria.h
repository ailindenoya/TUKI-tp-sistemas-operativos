#ifndef FUNCIONES_DE_MEMORIA_H_INCLUDED
#define FUNCIONES_DE_MEMORIA_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"
#include "commons/collections/list.h"

struct segmento{
    uint32_t id;
    uint32_t base; 
    uint32_t tamanio; 
};

typedef struct segmento segmento;
struct proceso{
    uint32_t pid; 
    segmento *tablaDeSegmentos;
};

typedef struct proceso proceso;
void buffer_empaquetar_tabla_de_segmentos(t_buffer* self, segmento* tablaDeSegs, int cantidadDeSegmentos);
segmento* segmento_crear(uint32_t id, uint32_t base, uint32_t tamanio);
void buffer_empaquetar_proceso_de_memoria(t_buffer* self, proceso* proceso, int cantidadDeSegmentos);
proceso* proceso_crear(uint32_t pid, int cantidadDeSegs);
void proceso_destruir(proceso* , int);
void buffer_empaquetar_lista_de_procesos_de_memoria(t_buffer* buffer,t_list* listaDeProcesos, int cantidadDeSegmentos);
void buffer_desempaquetar_tabla_de_segmentos(t_buffer* self, segmento* tablaDeSegs, int cantidadDeSegs);
uint32_t proceso_obtener_pid(proceso* unProceso);
segmento* proceso_obtener_tabla_de_segmentos(proceso* unProceso);
#endif