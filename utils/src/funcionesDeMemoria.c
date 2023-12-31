
#include "../include/funcionesDeMemoria.h"

#include <stdint.h>
#include <string.h>



void inicializar_tabla_de_segmentos(segmento* tablaDeSegmentos, int cantidadDeSegmentos){
    for(int i=0; i<cantidadDeSegmentos; i++){
        tablaDeSegmentos[i].base = -1;
        tablaDeSegmentos[i].id = -1;
        tablaDeSegmentos[i].tamanio = -1;
    }
}

segmento *segmento_crear(uint32_t id, uint32_t base, uint32_t tamanio){
    segmento* seg =  malloc(sizeof(*seg));
    seg->id = id;
    seg->base = base;
    seg->tamanio = tamanio;
    return seg;
}

void buffer_empaquetar_tabla_de_segmentos(t_buffer* self, segmento* tablaDeSegs, int cantidadDeSegmentos){
    segmento* tablaDeSegmentosAux = tablaDeSegs;
    for(int i=0; i<cantidadDeSegmentos; i++){
        buffer_empaquetar(self, &tablaDeSegmentosAux[i].id, sizeof(tablaDeSegmentosAux[i].id));
        buffer_empaquetar(self, &tablaDeSegmentosAux[i].base, sizeof(tablaDeSegmentosAux[i].base));
        buffer_empaquetar(self, &tablaDeSegmentosAux[i].tamanio, sizeof(tablaDeSegmentosAux[i].tamanio));
    }
}

void buffer_empaquetar_proceso_de_memoria(t_buffer* self, proceso* unProceso, int cantidadDeSegmentos){
    buffer_empaquetar(self,&unProceso->pid, sizeof(unProceso->pid));
    buffer_empaquetar_tabla_de_segmentos(self,unProceso->tablaDeSegmentos,cantidadDeSegmentos);
}

void buffer_desempaquetar_tabla_de_segmentos(t_buffer* self, segmento* tablaDeSegs, int cantidadDeSegs){
    segmento* tablaDeSegmentosAux = tablaDeSegs;
    for(int i=0; i<cantidadDeSegs; i++){
        buffer_desempaquetar(self, &(tablaDeSegmentosAux[i].id), sizeof(tablaDeSegmentosAux[i].id));
        buffer_desempaquetar(self, &(tablaDeSegmentosAux[i].base), sizeof(tablaDeSegmentosAux[i].base));
        buffer_desempaquetar(self, &(tablaDeSegmentosAux[i].tamanio), sizeof(tablaDeSegmentosAux[i].tamanio));
    }
}

void buffer_empaquetar_lista_de_procesos_de_memoria(t_buffer* bufferProcesos,t_list* listaDeProcesos, int cantidadDeSegmentos){
    uint32_t cantidadDeProcesos = list_size(listaDeProcesos);
    buffer_empaquetar(bufferProcesos, &cantidadDeProcesos, sizeof(cantidadDeProcesos));
    for (int i = 0; i < cantidadDeProcesos; i++){
        buffer_empaquetar_proceso_de_memoria(bufferProcesos, list_get(listaDeProcesos, i), cantidadDeSegmentos);
    }
}


proceso* proceso_crear(uint32_t pid, int cantidadDeSegs){
    proceso* procesoNuevo = malloc(sizeof(*procesoNuevo));
    procesoNuevo->pid = pid; 
    procesoNuevo->tablaDeSegmentos = malloc(sizeof(*(procesoNuevo->tablaDeSegmentos))*cantidadDeSegs);
    inicializar_tabla_de_segmentos(procesoNuevo->tablaDeSegmentos, cantidadDeSegs);
    return procesoNuevo;
}

void proceso_destruir(proceso* procesoADestruir, int cantidadDeSegmentos){
    for(int i = 1; i < cantidadDeSegmentos; i++) {
        free(&(procesoADestruir->tablaDeSegmentos[i]));
    }
    free(procesoADestruir);
}

uint32_t proceso_obtener_pid(proceso* unProceso){
    return unProceso->pid;
}

segmento* proceso_obtener_tabla_de_segmentos(proceso* unProceso){
    return unProceso->tablaDeSegmentos;
}
