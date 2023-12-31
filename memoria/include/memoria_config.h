#ifndef MEMORIA_CONFIG_H_INCLUDED
#define MEMORIA_CONFIG_H_INCLUDED

#include <errno.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/socket.h>

#include "../../utils/include/flagsParaCommons.h"
#include "../../utils/include/conexiones.h"
#include "../../utils/include/stream.h"

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>


typedef struct t_memoria_config t_memoria_config;

char* memoria_config_obtener_puerto_escucha(t_memoria_config*);
int memoria_config_obtener_retardo_memoria(t_memoria_config*);
int memoria_config_obtener_retardo_compactacion(t_memoria_config*);
int memoria_config_obtener_cantidad_de_segmentos(t_memoria_config*);
int memoria_config_obtener_tamanio_memoria(t_memoria_config*);
int memoria_config_obtener_tamanio_segmento_0(t_memoria_config*);
t_memoria_config* memoria_config_crear(char* memoriaConfigPath, t_log*);
bool memoria_config_es_algoritmo_asignacion_best(t_memoria_config* self);
bool memoria_config_es_algoritmo_asignacion_first(t_memoria_config* self);
bool memoria_config_es_algoritmo_asignacion_worst(t_memoria_config* self);



#endif //MEMORIA_CONFIG_H_INCLUDED