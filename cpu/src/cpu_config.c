#include "../cpu/include/cpu_config.h"

#include <stdlib.h>
#include <string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <stdlib.h>
#include <string.h>


t_log* cpuLogger;
t_cpu_config* cpuConfig;


t_log* cpuLogger;

struct t_cpu_config {
    char* IP;
    int RETARDO_INSTRUCCION;
    char* IP_MEMORIA;
    int PUERTO_MEMORIA;
    int PUERTO_ESCUCHA;
    int TAM_MAX_SEGMENTO;

    int SOCKET_MEMORIA;
};

t_cpu_config* cpuConfig;

static void cpu_iniciar_config(void* moduleConfig, t_config* tempCfg) {
    t_cpu_config* cpuConfig = (t_cpu_config*)moduleConfig;
    cpuConfig->IP = strdup(config_get_string_value(tempCfg, "IP"));
    cpuConfig->RETARDO_INSTRUCCION = config_get_int_value(tempCfg, "RETARDO_INSTRUCCION");
    cpuConfig->IP_MEMORIA = strdup(config_get_string_value(tempCfg, "IP_MEMORIA"));
    cpuConfig->PUERTO_MEMORIA = config_get_int_value(tempCfg, "PUERTO_MEMORIA");
    cpuConfig->TAM_MAX_SEGMENTO = config_get_int_value(tempCfg, "TAM_MAX_SEGMENTO");
    cpuConfig->SOCKET_MEMORIA = -1;
}


t_cpu_config* cpu_crear_config(char* cpuConfigPath, t_log* cpuLogger) {
    t_cpu_config* self = malloc(sizeof(*self));
    config_init(self, cpuConfigPath, cpuLogger, cpu_iniciar_config);
    return self;
}


char* cpu_config_obtener_ip_cpu(t_cpu_config* self) {
    return self->IP;
}

char* cpu_config_obtener_ip_memoria(t_cpu_config* self) {
    return self->IP_MEMORIA;
}

int cpu_config_obtener_retardo_instruccion(t_cpu_config* self) {
    return self->RETARDO_INSTRUCCION;
}

int cpu_config_obtener_puerto_memoria(t_cpu_config* self) {
    return self->PUERTO_MEMORIA;
}

int cpu_config_obtener_puerto_escucha(t_cpu_config* self) {
    return self->PUERTO_ESCUCHA;
}

int cpu_config_obtener_tam_max_segmento(t_cpu_config* self) {
    return self->TAM_MAX_SEGMENTO;
}

int cpu_config_obtener_socket_memoria(t_cpu_config* self) {
    return self->SOCKET_MEMORIA;
}

int cpu_config_setear_socket_memoria(t_cpu_config* self, int socket) {
    return self->SOCKET_MEMORIA = socket;
}

