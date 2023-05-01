#include "../include/contextoDeEjecucion.h"

#include <stdint.h>

extern t_log* cpuLogger;


struct t_contexto {
    uint32_t pid;
    uint32_t programCounter;
    t_list* instrucciones;
    char AX[4];
    char BX[4];
    char CX[4];
    char DX[4];

    char EAX[8];
    char EBX[8];
    char ECX[8];
    char EDX[8];

    char RAX[16];
    char RBX[16];
    char RCX[16];
    char RDX[16];
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

