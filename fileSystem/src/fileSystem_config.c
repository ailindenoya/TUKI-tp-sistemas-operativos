#include "../include/fileSystem_config.h"

#include <commons/config.h>
#include <commons/log.h>
#include <stdlib.h>
#include <string.h>
#include "../../utils/include/config_inicializacion.h"

t_log* fileSystemLogger;

struct t_fileSystem_config{

    char* IP_MEMORIA;
    char* PUERTO_MEMORIA;
    char* PUERTO_ESCUCHA;
    char* PATH_SUPERBLOQUE;
    char* PATH_BITMAP;
    char* PATH_BLOQUES;
    char* PATH_FCB;
    int   RETARDO_ACCESO_BLOQUE;
};

static void fileSystem_config_iniciar(void* moduleConfig, t_config* tempCfg){

    t_fileSystem_config* fileSystemConfig = (t_fileSystem_config*)moduleConfig;
    fileSystemConfig->IP_MEMORIA = strdup(config_get_string_value(tempCfg, "IP_MEMORIA"));
    fileSystemConfig->PUERTO_MEMORIA = strdup(config_get_string_value(tempCfg, "PUERTO_MEMORIA"));
    fileSystemConfig->PUERTO_ESCUCHA = strdup(config_get_string_value(tempCfg, "PUERTO_ESCUCHA"));
    fileSystemConfig->PATH_SUPERBLOQUE = strdup(config_get_string_value(tempCfg, "PATH_SUPERBLOQUE"));
    fileSystemConfig->PATH_BITMAP = strdup(config_get_string_value(tempCfg, "PATH_BITMAP"));
    fileSystemConfig->PATH_BLOQUES = strdup(config_get_string_value(tempCfg, "PATH_BLOQUES"));
    fileSystemConfig->PATH_FCB = strdup(config_get_string_value(tempCfg, "PATH_FCB"));
    fileSystemConfig->RETARDO_ACCESO_BLOQUE = config_get_int_value(tempCfg, "RETARDO_ACCESO_BLOQUE");
}

t_fileSystem_config* fileSystem_config_crear(char* fileSystemConfigPath, t_log* fileSystemLogger){
    t_fileSystem_config* self = malloc(self);
    config_iniciar(self, fileSystemConfigPath, fileSystemLogger, fileSystem_config_iniciar);
    return self;
}

// void fileSystem_config_destruir(t_fileSystem_config* self) {
//     free(self->IP_MEMORIA);
//     free(self->PUERTO_MEMORIA);
//     free(self->PUERTO_ESCUCHA);
//     free(self->PATH_SUPERBLOQUE);
//     free(self->PATH_BITMAP);
//     free(self->PATH_BLOQUES);
//     free(self->PATH_FCB);
//     free(self->RETARDO_ACCESO_BLOQUE);
//     free(self);
// }

char* fileSystem_config_obtener_ip_memoria(t_fileSystem_config* self){
    return self->IP_MEMORIA;
}
char* fileSystem_config_obtener_puerto_memoria(t_fileSystem_config* self){
    return self->PUERTO_MEMORIA;
}
char* fileSystem_config_obtener_puerto_escucha(t_fileSystem_config* self){
    return self->PUERTO_ESCUCHA;
}
char* fileSystem_config_obtener_path_superbloque(t_fileSystem_config* self){
    return self->PATH_SUPERBLOQUE;
}
char* fileSystem_config_obtener_path_bitmap(t_fileSystem_config* self){
    return self->PATH_BITMAP;
}
char* fileSystem_config_obtener_path_bloques(t_fileSystem_config* self){
    return self->PATH_BLOQUES;
}
char* fileSystem_config_obtener_path_fcb(t_fileSystem_config* self){
    return self->PATH_FCB;
}
int fileSystem_config_obtener_retardo_acceso_bloque(t_fileSystem_config* self){
    return self->RETARDO_ACCESO_BLOQUE;
}
