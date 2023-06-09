#include "../include/config_inicializacion.h"
#include <string.h>

int config_iniciar(void* config, char* pathAlConfig, t_log* logger,void (*config_initializer)(void* moduleConfig, t_config* configTemp)) {
    t_config* configTemp = config_create(pathAlConfig);
    if (NULL == configTemp) {
        log_error(logger, "el path \"%s\" no se encontro", pathAlConfig);
        return -1;
    }
    config_initializer(config, configTemp);
    log_info(logger, "Inicialización de campos correcta");
    config_destroy(configTemp);
    return 1;
}

char** copiar_array(char** arrayDeConfig) {
    char** arrayAux = arrayDeConfig;
    int dimensionDelArray = 0;
    while(*arrayAux != NULL) {
        dimensionDelArray++;
        arrayAux++;
    }
    char** arrayCopia = malloc(sizeof(*arrayCopia) * dimensionDelArray + 1);
    for(int i = 0; i < dimensionDelArray; i++) {
        arrayCopia[i] = strdup(arrayDeConfig[i]);
    }
    arrayCopia[dimensionDelArray] = NULL;
    return arrayCopia;
}