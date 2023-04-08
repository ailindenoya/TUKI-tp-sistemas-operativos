#include "../kernel/include/kernel_config.h"

#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <stdlib.h>
#include <string.h>


t_log* kernelLogger;


struct t_kernel_config {
  
    char* IP_MEMORIA;
    int PUERTO_MEMORIA;

    char* IP_FILESYSTEM;
    int PUERTO_FILESYSTEM;

    char* IP_CPU;
    int PUERTO_CPU;

    int PUERTO_ESCUCHA;
    char* ALGORITMO_PLANIFICACION;
    int ESTIMACION_INICIAL;
    int HRRN_ALFA;
    int GRADO_MAX_MULTIPROGRAMACION;
    t_list *RECURSOS; //Lista ordenada de los nombres de los recursos compartidos del sistema
    t_list *INSTANCIAS_RECURSOS; //Lista ordenada de la cantidad de unidades por recurso

};

t_kernel_config* kernelConfig;

static void kernel_iniciar_config(void* moduleConfig, t_config* tempCfg) {
    t_kernel_config* kernelConfig = (t_kernel_config*)moduleConfig;
    kernelConfig->IP_MEMORIA = strdup(config_get_string_value(tempCfg, "IP_MEMORIA"));
    kernelConfig->PUERTO_MEMORIA = strdup(config_get_string_value(tempCfg, "PUERTO_MEMORIA"));

    kernelConfig->IP_CPU = strdup(config_get_string_value(tempCfg, "IP_CPU"));
    kernelConfig->PUERTO_CPU = strdup(config_get_string_value(tempCfg, "PUERTO_CPU"));

    kernelConfig->IP_FILESYSTEM = strdup(config_get_string_value(tempCfg, "IP_FILESYSTEM"));
    kernelConfig->PUERTO_FILESYSTEM = strdup(config_get_string_value(tempCfg, "PUERTO_FILESYSTEM"));

    kernelConfig->PUERTO_ESCUCHA = strdup(config_get_string_value(tempCfg, "PUERTO_ESCUCHA"));
    kernelConfig->ALGORITMO_PLANIFICACION = strdup(config_get_string_value(tempCfg, "ALGORITMO_PLANIFICACION"));
    kernelConfig->ESTIMACION_INICIAL = config_get_double_value(tempCfg, "ESTIMACION_INICIAL");
    kernelConfig->HRRN_ALFA = config_get_double_value(tempCfg, "ALFA");
    kernelConfig->GRADO_MAX_MULTIPROGRAMACION = config_get_int_value(tempCfg, "GRADO_MULTIPROGRAMACION");
    kernelConfig->RECURSOS =  config_get_array_value(tempCfg, "RECURSOS");
    kernelConfig->INSTANCIAS_RECURSOS =  config_get_array_value(tempCfg, "INSTANCIAS_RECURSOS");
}

t_kernel_config* kernel_crear_config(char* kernelConfigPath, t_log* kernelLogger) {
    t_kernel_config* kernel = malloc(sizeof(*kernel));
    config_init(kernel, kernelConfigPath, kernelLogger, kernel_iniciar_config);
    return kernel;
}

void kernel_config_destruir(t_kernel_config* kernel) {
    free(kernel->IP_MEMORIA);
    free(kernel->PUERTO_MEMORIA);
    free(kernel->IP_CPU);
    free(kernel->PUERTO_CPU);
    free(kernel->PUERTO_FILESYSTEM);
    free(kernel->PUERTO_ESCUCHA);
    free(kernel->ALGORITMO_PLANIFICACION);
    free(kernel);
}


double kernel_config_obtener_est_inicial(t_kernel_config* self) {
    return self->ESTIMACION_INICIAL;
}

char* kernel_config_get_puerto_escucha(t_kernel_config* self) {
    return self->PUERTO_ESCUCHA;
}

char* kernel_config_obtener_ip_memoria(t_kernel_config* self) {
    return self->IP_MEMORIA;
}

char* kernel_config_obtener_puerto_memoria(t_kernel_config* self) {
    return self->PUERTO_MEMORIA;
}

char* kernel_config_obtener_ip_filesystem(t_kernel_config* self) {
    return self->IP_FILESYSTEM;
}

char* kernel_config_obtener_puerto_cpu(t_kernel_config* self) {
    return self->PUERTO_CPU;
}

char* kernel_config_obtener_ip_cpu(t_kernel_config* self) {
    return self->IP_CPU;
}

int kernel_config_obtener_grado_multiprogramacion(t_kernel_config* self) {
    return self->GRADO_MAX_MULTIPROGRAMACION;
}

double kernel_config_obtener_hrrn_alfa(t_kernel_config* self) {
    return self->HRRN_ALFA;
}

bool kernel_config_es_algoritmo_fifo(t_kernel_config* self) {
    return strcmp(self->ALGORITMO_PLANIFICACION, "FIFO") == 0;
}

bool kernel_config_es_algoritmo_hrrn(t_kernel_config* self) {
    return strcmp(self->ALGORITMO_PLANIFICACION, "HRRN") == 0;
}