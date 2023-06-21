#ifndef FUNCIONES_DE_MEMORIA_H_INCLUDED
#define FUNCIONES_DE_MEMORIA_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"

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
void buffer_desempaquetar_proceso_de_memoria(t_buffer* self, proceso* unProceso, int cantidadDeSegmentos);
proceso* proceso_crear(uint32_t pid, int cantidadDeSegs);
void proceso_destruir(proceso* procesoADestruir);
void buffer_desempaquetar_tabla_de_segmentos(t_buffer* self, segmento* tablaDeSegs, int cantidadDeSegs);
void buffer_desempaquetar_proceso_de_memoria(t_buffer* self, proceso* unProceso, int cantidadDeSegmentos);
#endif