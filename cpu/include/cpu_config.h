#include "../../utils/include/flagsParaCommons.h"
#include "../../utils/include/stream.h"
#include "../../utils/include/conexiones.h"
#include <sys/socket.h>
#include <commons/log.h>
#include <commons/config.h>

typedef struct t_cpu_config t_cpu_config;

int cpu_config_setear_socket_memoria(t_cpu_config* self, int socket);
void cpu_config_setear_socket_kernel(t_cpu_config* self, int socket);
void cpu_iniciar_config(void* moduleConfig, t_config* tempCfg);
t_cpu_config* cpu_config_crear(char* cpuConfigPath, t_log* cpuLogger) ;
char* cpu_config_obtener_ip_cpu(t_cpu_config* self);
char* cpu_config_obtener_ip_memoria(t_cpu_config* self);
char* cpu_config_obtener_retardo_instruccion(t_cpu_config* self);
char* cpu_config_obtener_puerto_memoria(t_cpu_config* self);
char* cpu_config_obtener_puerto_escucha(t_cpu_config* self);
char* cpu_config_obtener_tam_max_segmento(t_cpu_config* self) ;
int cpu_config_obtener_socket_memoria(t_cpu_config* self);
int cpu_config_setear_socket_memoria(t_cpu_config* self, int socket) ;
void cpu_config_setear_socket_kernel(t_cpu_config* self, int socket) ;