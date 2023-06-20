
#include "../include/funcionesDeMemoria.h"
#include <stdint.h>
#include <string.h>


segmento* crear_segmento(uint32_t id, uint32_t base, uint32_t tamanio){
    segmento* seg =  malloc(sizeof(*seg));
    seg->id = id;
    seg->base = base;
    seg->tamanio = tamanio;
    return seg;
}
void buffer_empaquetar_tabla(t_buffer* self, proceso* unProceso, int cantidadDeSegmentos){
    buffer_empaquetar(self,&unProceso->pid, sizeof(unProceso->pid));
    segmento** tablaDeSegmentosAux = unProceso->tablaDeSegmentos;
    for(int i=0; i<cantidadDeSegmentos; i++){
        buffer_empaquetar(self, &tablaDeSegmentosAux[i]->id, sizeof(tablaDeSegmentosAux[i]->id));
        buffer_empaquetar(self, &tablaDeSegmentosAux[i]->base, sizeof(tablaDeSegmentosAux[i]->base));
        buffer_empaquetar(self, &tablaDeSegmentosAux[i]->tamanio, sizeof(tablaDeSegmentosAux[i]->tamanio));
    }
}
void buffer_desempaquetar_tabla(t_buffer* self, proceso* unProceso, int cantidadDeSegmentos){
    
    buffer_desempaquetar(self,&unProceso->pid, sizeof(unProceso->pid));
    segmento** tablaDeSegmentosAux = unProceso->tablaDeSegmentos;
    for(int i=0; i<cantidadDeSegmentos; i++){
        buffer_desempaquetar(self, &tablaDeSegmentosAux[i]->id, sizeof(tablaDeSegmentosAux[i]->id));
        buffer_desempaquetar(self, &tablaDeSegmentosAux[i]->base, sizeof(tablaDeSegmentosAux[i]->base));
        buffer_desempaquetar(self, &tablaDeSegmentosAux[i]->tamanio, sizeof(tablaDeSegmentosAux[i]->tamanio));
    }
}
