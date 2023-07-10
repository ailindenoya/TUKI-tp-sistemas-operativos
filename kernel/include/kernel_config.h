#ifndef KERNEL_CONFIG_H_INCLUDED
#define KERNEL_CONFIG_H_INCLUDED

#include <errno.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/socket.h>

#include "../../utils/include/flagsParaCommons.h"
#include "../../utils/include/conexiones.h"
#include "../../utils/include/stream.h"
#include "../../utils/include/config_inicializacion.h"

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>



typedef struct t_kernel_config t_kernel_config;

t_kernel_config* kernel_config_crear(char* kernelConfigPath, t_log* kernelLogger) ;
void avisar_si_hay_error(int socket, char* tipo);
void intentar_establecer_conexion(int socket, char* tipo);
void kernel_config_destruir(t_kernel_config* self);
void kernel_destruir(t_kernel_config* kernelConfig, t_log* kernelLog);
void kernel_config_iniciar(void* moduleConfig, t_config* tempCfg);

char* kernel_config_obtener_ip_memoria(t_kernel_config* self) ;
char* kernel_config_obtener_puerto_memoria(t_kernel_config* self);
char* kernel_config_obtener_ip_filesystem(t_kernel_config* self) ;
char* kernel_config_obtener_puerto_filesystem(t_kernel_config* self) ;
char* kernel_config_obtener_ip_cpu(t_kernel_config* self) ;
char* kernel_config_obtener_puerto_cpu(t_kernel_config* self) ;
char* kernel_config_obtener_puerto_escucha(t_kernel_config* self) ;
char* kernel_config_obtener_ip_memoria(t_kernel_config* self);
bool kernel_config_es_algoritmo_hrrn(t_kernel_config* self) ;
bool kernel_config_es_algoritmo_fifo(t_kernel_config* self);
int kernel_config_obtener_grado_multiprogramacion(t_kernel_config* self);
double kernel_config_obtener_estimacion_inicial(t_kernel_config* self) ;
double kernel_config_obtener_hrrn_alfa(t_kernel_config* self);
void kernel_config_setear_socket_cpu(t_kernel_config* self, int socket);
void kernel_config_setear_socket_memoria(t_kernel_config* self, int socket);
void kernel_config_setear_socket_filesystem_peticiones(t_kernel_config* self, int socket);
void kernel_config_setear_socket_filesystem_desbloqueos(t_kernel_config* self, int socket);
char** kernel_config_obtener_recursos(t_kernel_config* self);
char** kernel_config_obtener_instancias_recursos(t_kernel_config* self);
int kernel_config_obtener_socket_cpu(t_kernel_config* self);
int kernel_config_obtener_socket_memoria(t_kernel_config* self);
int kernel_config_obtener_socket_filesystem_peticiones(t_kernel_config* self);
int kernel_config_obtener_socket_filesystem_desbloqueos(t_kernel_config* self);

#endif