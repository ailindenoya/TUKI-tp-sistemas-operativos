#include "../include/kernel_config.h"

t_log* kernelLogger;
t_kernel_config* kernelConfig;

struct t_kernel_config {
    char* IP_MEMORIA;
    char* PUERTO_MEMORIA;
    char* IP_FILESYSTEM;
    char* PUERTO_FILESYSTEM;
    char* IP_CPU;
    char* PUERTO_CPU;
    char* PUERTO_ESCUCHA;
    char* ALGORITMO_PLANIFICACION;
    double ESTIMACION_INICIAL;
    double HRRN_ALFA;
    int GRADO_MAX_MULTIPROGRAMACION;
    int SOCKET_CPU;
    int SOCKET_MEMORIA;
    int SOCKET_FILESYSTEM_PETICIONES;
    int SOCKET_FILESYSTEM_DESBLOQUEOS;
    int SOCKET_COMPACTACION;
    char** RECURSOS; //Lista ordenada de los nombres de los recursos compartidos del sistema 
    char** INSTANCIAS_RECURSOS; //Lista ordenada de la cantidad de unidades por recurso

};

void kernel_config_iniciar(void* moduleConfig, t_config* tempCfg) {
    t_kernel_config* kernelConfig = (t_kernel_config*)moduleConfig;
    kernelConfig->IP_MEMORIA = strdup(config_get_string_value(tempCfg, "IP_MEMORIA"));
    kernelConfig->PUERTO_MEMORIA = strdup(config_get_string_value(tempCfg, "PUERTO_MEMORIA"));
    kernelConfig->IP_FILESYSTEM = strdup(config_get_string_value(tempCfg, "IP_FILESYSTEM"));
    kernelConfig->PUERTO_FILESYSTEM = strdup(config_get_string_value(tempCfg, "PUERTO_FILESYSTEM"));
    kernelConfig->IP_CPU = strdup(config_get_string_value(tempCfg, "IP_CPU"));
    kernelConfig->PUERTO_CPU = strdup(config_get_string_value(tempCfg, "PUERTO_CPU"));
    kernelConfig->PUERTO_ESCUCHA = strdup(config_get_string_value(tempCfg, "PUERTO_ESCUCHA"));
    kernelConfig->ALGORITMO_PLANIFICACION = strdup(config_get_string_value(tempCfg, "ALGORITMO_PLANIFICACION"));
    kernelConfig->ESTIMACION_INICIAL = config_get_double_value(tempCfg, "ESTIMACION_INICIAL");
    kernelConfig->HRRN_ALFA = config_get_double_value(tempCfg, "HRRN_ALFA");
    kernelConfig->GRADO_MAX_MULTIPROGRAMACION = config_get_int_value(tempCfg, "GRADO_MAX_MULTIPROGRAMACION");
    kernelConfig->SOCKET_CPU = -1;
    kernelConfig->SOCKET_MEMORIA = -1;
    kernelConfig->SOCKET_FILESYSTEM_PETICIONES = -1;
    kernelConfig->SOCKET_FILESYSTEM_DESBLOQUEOS = -1;
    kernelConfig->SOCKET_COMPACTACION = -1;
    kernelConfig->RECURSOS = copiar_array(config_get_array_value(tempCfg,"RECURSOS"));
    kernelConfig->INSTANCIAS_RECURSOS = copiar_array(config_get_array_value(tempCfg,"INSTANCIAS_RECURSOS"));
}


t_kernel_config* kernel_config_crear(char* kernelConfigPath, t_log* kernelLogger) {
    t_kernel_config* self = malloc(sizeof(*self));
    config_iniciar(self, kernelConfigPath, kernelLogger, kernel_config_iniciar);
    return self;
}

void kernel_config_destruir(t_kernel_config* self) {
    free(self->IP_MEMORIA);
    free(self->PUERTO_MEMORIA);
    free(self->IP_CPU);
    free(self->PUERTO_CPU);
    free(self->IP_FILESYSTEM);
    free(self->PUERTO_FILESYSTEM);
    free(self->PUERTO_ESCUCHA);
    free(self->ALGORITMO_PLANIFICACION);
    free(self->RECURSOS);
    free(self->INSTANCIAS_RECURSOS);
    free(self);
}
char* kernel_config_obtener_algoritmo(t_kernel_config* self){
    return self->ALGORITMO_PLANIFICACION;
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
char* kernel_config_obtener_puerto_filesystem(t_kernel_config* self) {
    return self->PUERTO_FILESYSTEM;
}
char* kernel_config_obtener_ip_cpu(t_kernel_config* self) {
    return self->IP_CPU;
}
char* kernel_config_obtener_puerto_cpu(t_kernel_config* self) {
    return self->PUERTO_CPU;
}
char* kernel_config_obtener_puerto_escucha(t_kernel_config* self) {
    return self->PUERTO_ESCUCHA;
}
bool kernel_config_es_algoritmo_hrrn(t_kernel_config* self) {
    return strcmp(self->ALGORITMO_PLANIFICACION, "HRRN") == 0;
}
bool kernel_config_es_algoritmo_fifo(t_kernel_config* self) {
    return strcmp(self->ALGORITMO_PLANIFICACION, "FIFO") == 0;
}
int kernel_config_obtener_grado_multiprogramacion(t_kernel_config* self) {
    return self->GRADO_MAX_MULTIPROGRAMACION;
}
double kernel_config_obtener_estimacion_inicial(t_kernel_config* self) {
    return self->ESTIMACION_INICIAL;
}
double kernel_config_obtener_hrrn_alfa(t_kernel_config* self) {
    return self->HRRN_ALFA;
}
int kernel_config_obtener_socket_cpu(t_kernel_config* self) {
    return self->SOCKET_CPU;
}
int kernel_config_obtener_socket_memoria(t_kernel_config* self) {
    return self->SOCKET_MEMORIA;
}
int kernel_config_obtener_socket_compactacion(t_kernel_config* self) {
    return self->SOCKET_COMPACTACION;
}
void kernel_config_setear_socket_compactacion(t_kernel_config* self, int socket) {
    self->SOCKET_COMPACTACION = socket;
}
int kernel_config_obtener_socket_filesystem_peticiones(t_kernel_config* self) {
    return self->SOCKET_FILESYSTEM_PETICIONES;
}
int kernel_config_obtener_socket_filesystem_desbloqueos(t_kernel_config* self) {
    return self->SOCKET_FILESYSTEM_DESBLOQUEOS;
}
char** kernel_config_obtener_recursos(t_kernel_config* self){
    return self->RECURSOS;
}
char** kernel_config_obtener_instancias_recursos(t_kernel_config* self){
    return self->INSTANCIAS_RECURSOS;
}
void kernel_config_setear_socket_cpu(t_kernel_config* self, int socket) {
     self->SOCKET_CPU = socket;
}
void kernel_config_setear_socket_memoria(t_kernel_config* self, int socket) {
     self->SOCKET_MEMORIA = socket;
}
void kernel_config_setear_socket_filesystem_peticiones(t_kernel_config* self, int socket) {
     self->SOCKET_FILESYSTEM_PETICIONES = socket;
}
void kernel_config_setear_socket_filesystem_desbloqueos(t_kernel_config* self, int socket) {
     self->SOCKET_FILESYSTEM_DESBLOQUEOS = socket;
}
