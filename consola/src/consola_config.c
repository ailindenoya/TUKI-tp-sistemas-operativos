#include "../include/consola_config.h"


struct t_consola_config {
    char* IP_KERNEL;
    char* PUERTO_KERNEL;
};
void consola_iniciar_config(void* configurarModulo, t_config* tempCfg);

void consola_iniciar_config(void* configurarModulo, t_config* tempCfg) {
    t_consola_config* consolaConfig = (t_consola_config*)configurarModulo;
    consolaConfig->IP_KERNEL = strdup(config_get_string_value(tempCfg, "IP_KERNEL"));
    consolaConfig->PUERTO_KERNEL = strdup(config_get_string_value(tempCfg, "PUERTO_KERNEL"));
}

t_consola_config* consola_crear_config(char* consolaConfigPath, t_log* consolaLogger) {
    t_consola_config* consola = malloc(sizeof(*consola));
    config_iniciar(consola, consolaConfigPath, consolaLogger, consola_iniciar_config);
    return consola;
}

char* consola_config_obtener_kernel_IP(t_consola_config* self) {
    return self->IP_KERNEL;
}

char* consola_config_obtener_kernel_PUERTO(t_consola_config* self) {
    return self->PUERTO_KERNEL;
}

void consola_config_destruir(t_consola_config* consola_config) {
    free(consola_config->IP_KERNEL);
    free(consola_config->PUERTO_KERNEL);
    free(consola_config);
}