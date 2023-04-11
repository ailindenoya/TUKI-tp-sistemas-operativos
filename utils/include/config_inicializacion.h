
#ifndef CONFIG_INICIALIZACION_H_INCLUDED
#define CONFIG_INICIALIZACION_H_INCLUDED


#include <commons/config.h>
#include <stddef.h>
#include <commons/log.h>

int config_iniciar(void* config, char* pathAlConfig, t_log* logger,void (*config_initializer)(void* moduleConfig, t_config* configTemp));

#endif