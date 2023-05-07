#include "../include/contextoDeEjecucion.h"


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
    list_destroy_and_destroy_elements(self->instrucciones, instruccion_destroy);
    free(self);
}

t_list* contexto_obtener_instrucciones(t_contexto* self){
    return self->instrucciones;
}

char* contexto_obtener_AX(t_contexto* self){
   return  self->AX;
}

void contexto_setear_AX(t_contexto* self, char* string){
   return  self->AX = string;
}

char* contexto_obtener_BX(t_contexto* self){
   return  self->BX;
}
void contexto_setear_BX(t_contexto* self, char* string){
   return  self->BX = string;
}

char* contexto_obtener_CX(t_contexto* self){
   return  self->CX;
}

void contexto_setear_CX(t_contexto* self, char* string){
   return  self->CX = string;
}

char* contexto_obtener_DX(t_contexto* self){
   return  self->DX;
}

void contexto_setear_AX(t_contexto* self, char* string){
   return  self->DX = string;
}

char* contexto_obtener_EAX(t_contexto* self){
   return  self->EAX;
}

void contexto_setear_EAX(t_contexto* self, char* string){
   return  self->EAX = string;
}
char* contexto_obtener_EBX(t_contexto* self){
   return  self->EBX;
}

void contexto_setear_EBX(t_contexto* self, char* string){
   return  self->EBX = string;
}
char* contexto_obtener_ECX(t_contexto* self){
   return  self->ECX;
}

void contexto_setear_ECX(t_contexto* self, char* string){
   return  self->ECX = string;
}
char* contexto_obtener_EDX(t_contexto* self){
   return  self->EDX;
}

void contexto_setear_EDX(t_contexto* self, char* string){
   return  self->EDX = string;
}
char* contexto_obtener_RAX(t_contexto* self){
   return  self->RAX;
}
void contexto_setear_RAX(t_contexto* self, char* string){
   return  self->RAX = string;
}
char* contexto_obtener_RBX(t_contexto* self){
   return  self->RBX;
}
void contexto_setear_RBX(t_contexto* self, char* string){
   return  self->RBX = string;
}
char* contexto_obtener_RCX(t_contexto* self){
   return  self->RCX;
}
void contexto_setear_RCX(t_contexto* self, char* string){
   return  self->RCX = string;
}
char* contexto_obtener_RDX(t_contexto* self){
   return  self->RAX;
}
void contexto_setear_RDX(t_contexto* self, char* string){
   return  self->RAX = string;
}