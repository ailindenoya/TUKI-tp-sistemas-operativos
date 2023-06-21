
#include "../include/funcionesDeMemoria.h"
#include <stdint.h>
#include <string.h>


segmento* segmento_crear(uint32_t id, uint32_t base, uint32_t tamanio){
    segmento* seg =  malloc(sizeof(*seg));
    seg->id = id;
    seg->base = base;
    seg->tamanio = tamanio;
    return seg;
}

void buffer_empaquetar_tabla_de_segmentos(t_buffer* self, segmento** tablaDeSegs, int cantidadDeSegmentos){
    segmento** tablaDeSegmentosAux = tablaDeSegs;
    for(int i=0; i<cantidadDeSegmentos; i++){
        buffer_empaquetar(self, &tablaDeSegmentosAux[i]->id, sizeof(tablaDeSegmentosAux[i]->id));
        buffer_empaquetar(self, &tablaDeSegmentosAux[i]->base, sizeof(tablaDeSegmentosAux[i]->base));
        buffer_empaquetar(self, &tablaDeSegmentosAux[i]->tamanio, sizeof(tablaDeSegmentosAux[i]->tamanio));
    }
}

void buffer_empaquetar_proceso_de_memoria(t_buffer* self, proceso* unProceso, int cantidadDeSegmentos){
    buffer_empaquetar(self,&unProceso->pid, sizeof(unProceso->pid));
    buffer_empaquetar_tabla_de_segmentos(self,&unProceso->tablaDeSegmentos,cantidadDeSegmentos);
}

void buffer_desempaquetar_tabla_de_segmentos(t_buffer* self, segmento** tablaDeSegs, int cantidadDeSegs){
    segmento** tablaDeSegmentosAux = tablaDeSegs;
    for(int i=0; i<cantidadDeSegmentos; i++){
        buffer_desempaquetar(self, &tablaDeSegmentosAux[i]->id, sizeof(tablaDeSegmentosAux[i]->id));
        buffer_desempaquetar(self, &tablaDeSegmentosAux[i]->base, sizeof(tablaDeSegmentosAux[i]->base));
        buffer_desempaquetar(self, &tablaDeSegmentosAux[i]->tamanio, sizeof(tablaDeSegmentosAux[i]->tamanio));
    }
}

void buffer_desempaquetar_proceso_de_memoria(t_buffer* self, proceso* unProceso, int cantidadDeSegmentos){
    buffer_desempaquetar(self,&unProceso->pid, sizeof(unProceso->pid));
    buffer_desempaquetar_tabla_de_segmentos(self,&unProceso->tablaDeSegmentos, cantidadDeSegmentos);
}

proceso* proceso_crear(uint32_t pid, int cantidadDeSegs){
    proceso* procesoNuevo = malloc(sizeof(*procesoNuevo));
    procesoNuevo->pid = pid; 
    procesoNuevo->tablaDeSegmentos = malloc(sizeof(*(procesoNuevo->tablaDeSegmentos))*cantidadDeSegs);
    return procesoNuevo;
}

void proceso_destruir(proceso* procesoADestruir){
    free(procesoADestruir->tablaDeSegmentos);
    free(procesoADestruir);
}