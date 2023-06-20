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
    segmento **tablaDeSegmentos;
};

typedef struct proceso proceso;

segmento* crear_segmento(uint32_t id, uint32_t base, uint32_t tamanio);
void buffer_empaquetar_tabla(t_buffer* self, proceso* proceso, int cantidadDeSegmentos);
void buffer_desempaquetar_tabla(t_buffer* self, proceso* unProceso, int cantidadDeSegmentos);


#endif