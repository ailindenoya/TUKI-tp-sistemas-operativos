#ifndef INSTRUCCIONES_H_INCLUDED
#define INSTRUCCIONES_H_INCLUDED

#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/log.h>

#include <stdlib.h>
#include "../include/flagsParaCommons.h"
#include "../include/buffer.h"


typedef struct t_instruccion t_instruccion;
t_instruccion* instruccion_crear(t_tipo_instruccion tipoInstruccion, char* parametroUno, char* parametroDos, char* parametroTres);
void instruccion_destruir(void* selfVoid);
t_tipo_instruccion instruccion_obtener_tipo_instruccion(t_instruccion* self);
char* instruccion_obtener_parametro1(t_instruccion* self);
char* instruccion_obtener_parametro2(t_instruccion* self);
char* instruccion_obtener_parametro3(t_instruccion* self);
#endif