#ifndef CPU_CONFIG_H_INCLUDED
#define CPU_CONFIG_H_INCLUDED

#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <sys/socket.h>
#include "../../utils/include/flagsParaCommons.h"
#include "../../utils/include/stream.h"
#include "../../utils/include/conexiones.h"
#include "../../utils/include/config_inicializacion.h"



typedef struct t_cpu_config t_cpu_config;

void cpu_iniciar_config(void* moduleConfig, t_config* tempCfg);

void cpu_config_setear_socket_memoria(t_cpu_config* self, int socket);
void cpu_config_setear_socket_kernel(t_cpu_config* self, int socket);
t_cpu_config* cpu_config_crear(char* cpuConfigPath, t_log* cpuLogger) ;
char* cpu_config_obtener_ip_memoria(t_cpu_config* self);
uint32_t cpu_config_obtener_retardo_instruccion(t_cpu_config* self);
char* cpu_config_obtener_puerto_memoria(t_cpu_config* self);
char* cpu_config_obtener_puerto_escucha(t_cpu_config* self);
int cpu_config_obtener_tam_max_segmento(t_cpu_config* self) ;
int cpu_config_obtener_socket_memoria(t_cpu_config* self);
void cpu_config_setear_socket_kernel(t_cpu_config* self, int socket) ;
int cpu_config_obtener_socket_kernel(t_cpu_config* self);


#endif