#include "../include/cpu_config.h"
#include<string.h>

t_log* cpuLogger;
t_cpu_config* cpuConfig;


t_log* cpuLogger;

struct t_cpu_config {
    char* IP;
    uint32_t RETARDO_INSTRUCCION; // deberia ser uint32_t?
    char* IP_MEMORIA;
    char* PUERTO_MEMORIA;
    char* PUERTO_ESCUCHA;
    char* TAM_MAX_SEGMENTO;

    int SOCKET_MEMORIA;
    int SOCKET_KERNEL;
};

t_cpu_config* cpuConfig;
static void cpu_iniciar_config(void* moduleConfig, t_config* tempCfg);

static void cpu_iniciar_config(void* moduleConfig, t_config* tempCfg) {
    t_cpu_config* cpuConfig = (t_cpu_config*)moduleConfig;
    cpuConfig->IP = config_get_string_value(tempCfg, "IP");
    cpuConfig->RETARDO_INSTRUCCION = config_get_int_value(tempCfg, "RETARDO_INSTRUCCION");
    cpuConfig->IP_MEMORIA = config_get_string_value(tempCfg, "IP_MEMORIA");
    cpuConfig->PUERTO_MEMORIA = config_get_string_value(tempCfg, "PUERTO_MEMORIA");
    cpuConfig->TAM_MAX_SEGMENTO = config_get_string_value(tempCfg, "TAM_MAX_SEGMENTO");
    cpuConfig->SOCKET_MEMORIA = -1;
    cpuConfig->SOCKET_KERNEL = -1;
}


t_cpu_config* cpu_config_crear(char* cpuConfigPath, t_log* cpuLogger) {
    t_cpu_config* self = malloc(sizeof(*self));
    config_iniciar(self, cpuConfigPath, cpuLogger, cpu_iniciar_config);
    return self;
}


char* cpu_config_obtener_ip_cpu(t_cpu_config* self) {
    return self->IP;
}
char* cpu_config_obtener_ip_memoria(t_cpu_config* self) {
    return self->IP_MEMORIA;
}
uint32_t cpu_config_obtener_retardo_instruccion(t_cpu_config* self) {
    return self->RETARDO_INSTRUCCION;
}
char* cpu_config_obtener_puerto_memoria(t_cpu_config* self) {
    return self->PUERTO_MEMORIA;
}
char* cpu_config_obtener_puerto_escucha(t_cpu_config* self) {
    return self->PUERTO_ESCUCHA;
}
char* cpu_config_obtener_tam_max_segmento(t_cpu_config* self) {
    return self->TAM_MAX_SEGMENTO;
}
int cpu_config_obtener_socket_memoria(t_cpu_config* self) {
    return self->SOCKET_MEMORIA;
}
int cpu_config_setear_socket_memoria(t_cpu_config* self, int socket) {
    return self->SOCKET_MEMORIA = socket;
}
void cpu_config_setear_socket_kernel(t_cpu_config* self, int socket) {
    self->SOCKET_KERNEL = socket;
}

