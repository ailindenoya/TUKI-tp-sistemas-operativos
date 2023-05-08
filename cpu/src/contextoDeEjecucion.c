#include "../include/contextoDeEjecucion.h"


extern t_log* cpuLogger;

struct t_contexto {
    uint32_t pid;
    uint32_t programCounter;
    t_list* instrucciones;
};

t_contexto* crear_contexto(uint32_t pid, uint32_t programCounter){
   t_contexto* contexto = malloc(sizeof(*contexto));
   contexto->pid = pid; 
   contexto->programCounter = programCounter; 
   contexto->instrucciones = NULL;
   return contexto;
}

uint32_t contexto_obtener_pid(t_contexto* self) {
    return self->pid;
}
uint32_t contexto_obtener_program_counter(t_contexto* self) {
    return self->programCounter;
}

void contexto_setear_program_counter(t_contexto* self, uint32_t programCounter) {
    self->programCounter = programCounter;
}

void contexto_destruir(t_contexto* self) {
    list_destroy_and_destroy_elements(self->instrucciones, instruccion_destruir);
    free(self);
}

t_list* contexto_obtener_instrucciones(t_contexto* self){
    return self->instrucciones;
}