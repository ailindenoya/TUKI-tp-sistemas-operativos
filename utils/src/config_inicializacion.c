#include "../include/config_inicializacion.h"

int config_iniciar(void* config, char* pathAlConfig, t_log* logger,void (*config_initializer)(void* moduleConfig, t_config* configTemp)) {
    t_config* configTemp = config_create(pathAlConfig);
    if (NULL == configTemp) {
        log_error(logger, "el path \"%s\" no se encontro", pathAlConfig);
        return -1;
    }
    config_initializer(config, configTemp);
    log_info(logger, "Inicialización de campos correcta");
    //config_destroy(configTemp);    //TODO
    return 1;
}
