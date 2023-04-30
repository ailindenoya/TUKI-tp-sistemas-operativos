#include <stdlib.h>
#include <stdbool.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <pthread.h>
#include "../../utils/include/flagsParaCommons.h"
#include "../../utils/include/stream.h"
#include "../../kernel/include/estado.h"
#include "../include/cpu_config.h"
#include "contextoDeEjecucion.h"
#include "../../kernel/include/estado.h"


extern t_log* cpuLogger;
extern t_contexto contextoEjecucion;

void copiarStringAVector(char* string, char* vector, int tamanioDeRegistro);
void ejecutar_SET(char* reg, char* param);
void ejecutar_YIELD();
void ejecutar_EXIT();
bool cpu_exec_instruction(t_contexto* pcb, t_tipo_instruccion tipoInstruccion, char* parametro1, char* parametro2, char* parametro3);