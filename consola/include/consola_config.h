#ifndef CONSOLA_CONFIG_H_INCLUDED
#define CONSOLA_CONFIG_H_INCLUDED

#include <commons/config.h>
#include <commons/log.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/include/flagsParaCommons.h"
#include "../utils/include/stream.h"
#include "../utils/src/conexiones.c"

typedef struct t_consola_config t_consola_config;
char *consola_config_obtener_kernel_IP(t_consola_config *);
char *consola_config_obtener_kernel_PUERTO(t_consola_config *);
t_consola_config *consola_crear_config(char *consolaConfigPath, t_log *);
void consola_config_destroy(t_consola_config *);

#endif