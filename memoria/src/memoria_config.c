#include "../include/memoria_config.h"

#include <commons/config.h>
#include <commons/log.h>
#include <stdlib.h>
#include <string.h>
#include "../../utils/include/config_inicializacion.h"

t_log* memoriaLogger;
t_memoria_config* memoriaConfig;

struct t_memoria_config {
  
    char* PUERTO_ESCUCHA;
    int TAM_MEMORIA;
    int TAM_SEGMENTO;
    int CANT_SEGMENTOS;
    int RETARDO_MEMORIA;
    int RETARDO_COMPACTACION;
    char* ALGORITMO_ASIGNACION;
};


static void memoria_iniciar_config(void* moduleConfig, t_config* tempCfg) {
    t_memoria_config* memoriaConfig = (t_memoria_config*)moduleConfig;
    memoriaConfig->PUERTO_ESCUCHA = strdup(config_get_string_value(tempCfg, "PUERTO_ESCUCHA"));
    memoriaConfig->TAM_MEMORIA = config_get_int_value(tempCfg, "TAM_MEMORIA");
    memoriaConfig->TAM_SEGMENTO = config_get_int_value(tempCfg, "TAM_SEGMENTO");
    memoriaConfig->CANT_SEGMENTOS = config_get_int_value(tempCfg, "CANT_SEGMENTOS");
    memoriaConfig->RETARDO_MEMORIA = config_get_int_value(tempCfg, "RETARDO_MEMORIA");
    memoriaConfig->ALGORITMO_ASIGNACION= strdup(config_get_string_value(tempCfg, "ALGORITMO_ASIGNACION"));
    memoriaConfig->RETARDO_COMPACTACION = config_get_int_value(tempCfg, "RETARDO_COMPACTACION");
}

t_memoria_config* memoria_config_crear(char* memoriaConfigPath, t_log* memoriaLogger) {
    t_memoria_config * self  = (t_memoria_config *) malloc(sizeof(t_memoria_config));
    //t_memoria_config* self = malloc(sizeof(*self));
    config_iniciar(self, memoriaConfigPath, memoriaLogger, memoria_iniciar_config);
    return self;
}

/*
char* memoria_config_get_ip_escucha(t_memoria_config* self) {
    return self->IP_ESCUCHA;
}*/

char* memoria_config_obtener_puerto_escucha(t_memoria_config* self) {
    return self->PUERTO_ESCUCHA;
}

int memoria_config_obtener_tamanio_memoria(t_memoria_config* self) {
    return self->TAM_MEMORIA;
}

int memoria_config_obtener_tamanio_segmento(t_memoria_config* self) {
    return self->TAM_SEGMENTO;
}

int memoria_config_obtener_entradas_por_tabla(t_memoria_config* self) {
    return self->CANT_SEGMENTOS;
}

int memoria_config_obtener_retardo_compactacion(t_memoria_config* self) {
    return self->RETARDO_COMPACTACION;
}

int memoria_config_obtener_retardo_memoria(t_memoria_config* self) {
    return self->RETARDO_MEMORIA;
}

/*
int memoria_config_obtener_procesos_totales(t_memoria_config* self) {
    return self->TAM_MEMORIA / (self->TAM_PAGINA * self->MARCOS_POR_PROCESO);
}*/


bool memoria_config_es_algoritmo_asignacion(t_memoria_config* self) {
    return strcmp(self->ALGORITMO_ASIGNACION, "BEST") == 0;
}

/*
bool memoria_config_es_algoritmo_sustitucion_clock_modificado(t_memoria_config* self) {
    return strcmp(self->ALGORITMO_ASIGNACION, "CLOCK-M") == 0;
}*/