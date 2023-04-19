#include "../include/fileSystem_config.h"

#define LOGS_FILESYSTEM "bin/fileSystem.log"
#define MODULO_FILESYSTEM "fileSystem"
#define NUMBER_OF_ARGS_REQUIRED 2

extern t_log* fileSystemLogger;
extern t_fileSystem_config* fileSystemConfig;

void fileSystem_destruir(t_fileSystem_config* fileSystemConfig, t_log* fileSystemLogger) {
  fileSystem_config_destruir(fileSystemConfig);
 log_destroy(fileSystemLogger);
}

void avisar_si_hay_error(int socket, char* tipo){
    if (socket == -1) {
        log_error(fileSystemLogger, "no se pudo establecer conexion inicial con %s", tipo);
        fileSystem_destruir(fileSystemConfig, fileSystemLogger);
        exit(-1);
    }
}

void handshake_kernel(int socketKernel){
    uint8_t respuestaKERNEL = stream_recibir_header(socketKernel);
    stream_recibir_buffer_vacio(socketKernel);
    if (respuestaKERNEL != HANDSHAKE_kernel) {
        log_error(fileSystemLogger, "error al intentar establecer HANDSHAKE inicial con kernel");
        log_destroy(fileSystemLogger);
        exit(-1);
    }
    stream_enviar_buffer_vacio(socketKernel, HANDSHAKE_puede_continuar);
    log_info(fileSystemLogger, "conexion con kernel establecida");
}

int main(int argc, char* argv[]){

    fileSystemLogger = log_create(LOGS_FILESYSTEM, MODULO_FILESYSTEM, true, LOG_LEVEL_DEBUG);

    fileSystemConfig = fileSystem_config_crear(argv[1], fileSystemLogger);

    // intentar establecer conexion estaba MAL xq hacia handshake con algo que nqv, hay que rehacer esta parte
}

