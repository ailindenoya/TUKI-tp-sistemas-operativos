
#ifndef KERNEL_CONFIG_H_INCLUDED
#define KERNEL_CONFIG_H_INCLUDED

#include <errno.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/socket.h>
#include "../utils/include/flagsParaCommons.h"
#include "../utils/src/conexiones.c"
#include "../utils/include/stream.h"
#include <commons/log.h>

typedef struct t_kernel_config t_kernel_config;
t_kernel_config* kernel_config_create(char* kernelConfigPath, t_log*);

#endif