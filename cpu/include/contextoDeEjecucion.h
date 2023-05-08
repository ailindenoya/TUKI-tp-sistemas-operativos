#ifndef CONTEXTODEEJECUCION_H_INCLUDED
#define CONTEXTODEEJECUCION_H_INCLUDED

#include <commons/collections/list.h>
#include <stdint.h>
#include "cpu_config.h"
#include <commons/log.h>
#include "../../utils/include/instrucciones.h"

typedef struct t_contexto t_contexto;

t_contexto* crear_contexto(uint32_t pid, uint32_t programCounter);
void contexto_destruir(t_contexto* self);
uint32_t contexto_obtener_pid(t_contexto* self);
uint32_t contexto_obtener_program_counter(t_contexto* self);
void contexto_setear_program_counter(t_contexto* self, uint32_t programCounter);
void contexto_destruir(t_contexto* self);
t_list* contexto_obtener_instrucciones(t_contexto* self);
#endif