#ifndef COMUNICACIONKERNEL_H_INCLUDED
#define COMUNICACIONKERNEL_H_INCLUDED

#include <stdlib.h>
#include <stdbool.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <pthread.h>
#include "../../utils/include/flagsParaCommons.h"
#include "../../utils/include/stream.h"
#include "../../utils/include/instrucciones.h"

#include "../../kernel/include/estado.h"
#include "../include/cpu_config.h"
#include "contextoDeEjecucion.h"
#include "../../kernel/include/estado.h"


extern t_log* cpuLogger;

bool cpu_ejecutar_instrucciones(t_contexto* pcb, t_tipo_instruccion tipoInstruccion, char* parametro1, char* parametro2, char* parametro3);

char* contexto_obtener_AX(t_contexto* self);
void contexto_setear_AX(t_contexto* self, char* string);



char* contexto_obtener_BX(t_contexto* self);
void contexto_setear_BX(t_contexto* self, char* string);

char* contexto_obtener_CX(t_contexto* self);
void contexto_setear_CX(t_contexto* self, char* string);

char* contexto_obtener_DX(t_contexto* self);

void contexto_setear_AX(t_contexto* self, char* string);

char* contexto_obtener_EAX(t_contexto* self);

void contexto_setear_EAX(t_contexto* self, char* string);
char* contexto_obtener_EBX(t_contexto* self);

void contexto_setear_EBX(t_contexto* self, char* string);
char* contexto_obtener_ECX(t_contexto* self);

void contexto_setear_ECX(t_contexto* self, char* string);
char* contexto_obtener_EDX(t_contexto* self);

void contexto_setear_EDX(t_contexto* self, char* string);
char* contexto_obtener_RAX(t_contexto* self);
void contexto_setear_RAX(t_contexto* self, char* string);
char* contexto_obtener_RBX(t_contexto* self);
void contexto_setear_RBX(t_contexto* self, char* string);
char* contexto_obtener_RCX(t_contexto* self);
void contexto_setear_RCX(t_contexto* self, char* string);
char* contexto_obtener_RDX(t_contexto* self);
void contexto_setear_RDX(t_contexto* self, char* string);





void copiarStringAVector(char* string, char* vector, int tamanioDeRegistro);
void ejecutar_SET(char* reg, char* param, t_contexto* pcb );
void ejecutar_YIELD(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_EXIT(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_F_CLOSE(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_F_OPEN(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_F_READ(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_F_WRITE(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_F_SEEK(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_F_TRUNCATE(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_MOV_IN(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_MOV_OUT(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_CREATE_SEGMENT(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_DELETE_SEGMENT(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_WAIT(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_SIGNAL(t_contexto* pcb,uint32_t programCounterActualizado);

t_instruccion* cpu_fetch_instruccion(t_contexto* pcb);

#endif