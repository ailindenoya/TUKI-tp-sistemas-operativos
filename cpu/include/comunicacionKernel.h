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
bool cpu_ejecutar_ciclos_de_instruccion(t_contexto* pcb) ;
void dispatch_peticiones_de_kernel(void) ;
void atender_peticiones_de_kernel(void);

void copiarStringAVector(char* string, char* vector, int tamanioDeRegistro);
void ejecutar_SET(t_contexto* pcb, char* reg, char* param);
void ejecutar_YIELD(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_EXIT(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_F_CLOSE(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_F_OPEN(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_F_READ(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_F_WRITE(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_F_SEEK(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_F_TRUNCATE(t_contexto* pcb,uint32_t programCounterActualizado);
void ejecutar_MOV_IN(t_contexto* pcb,uint32_t programCounterActualizado, char* reg, char* dirLogica);
void ejecutar_MOV_OUT(t_contexto* pcb,uint32_t programCounterActualizado, char* dirLogica, char* regALeer);
void ejecutar_CREATE_SEGMENT(t_contexto* pcb,uint32_t programCounterActualizado, char* Idsegmento, char* tamanio);
void ejecutar_WAIT(t_contexto* pcb,uint32_t programCounterActualizado, char* recurso);
void ejecutar_SIGNAL(t_contexto* pcb,uint32_t programCounterActualizado, char* recurso);
void ejecutar_IO(t_contexto* pcb, uint32_t programCounterActualizado, char* tiempoDeBloqueo);

t_instruccion* cpu_fetch_instruccion(t_contexto* pcb);

#endif