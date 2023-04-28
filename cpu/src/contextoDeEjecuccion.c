#include "../include/contextoDeEjecucion.h"

#include <stdint.h>

extern t_log* cpuLogger;


struct t_contexto {
    uint32_t pid;
    uint32_t programCounter;
    t_list* instrucciones;
};


t_contexto* crear_contexto(uint32_t pid, uint32_t programCounter){
   t_contexto* contexto = malloc(sizeof(*self));
   contexto->pid = pid; 
   contexto->programCounter = programCounter; 
   contexto->instrucciones = NULL;
   return contexto;
}


void contexto_destruir(t_contexto* self) {
    list_destroy_and_destroy_elements(self->instrucciones, instruccion_destroy);
    free(self);
}

