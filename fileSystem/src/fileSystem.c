#include "../include/fileSystem_config.h"

#define LOGS_FILESYSTEM "../bin/fileSystem.log"
#define MODULO_FILESYSTEM "fileSystem"

extern t_log* fileSystemLogger;
extern t_fileSystem_config* fileSystemConfig;

// static void fileSystem_destruir(t_fileSystem_config* fileSystemConfig, t_log* fileSystemLogger) {
//     fileSystem_config_destruir(fileSystemConfig);
//     log_destroy(fileSystemLogger);
// }

void avisar_si_hay_error(int socket, char* tipo){
    if (socket == -1) {
        log_error(fileSystemLogger, "no se pudo establecer conexion inicial con %s", tipo);
        //fileSystem_destruir(fileSystemConfig, fileSystemLogger);
        exit(-1);
    }
}

void intentar_establecer_conexion(int socket, char* tipo){
    stream_enviar_buffer_vacio(socket, HANDSHAKE_interrupcion);
    uint8_t respuestaInterrupcion = stream_recibir_header(socket);
    stream_recibir_buffer_vacio(respuestaInterrupcion);
    if (respuestaInterrupcion != HANDSHAKE_puede_continuar) {
        log_error(fileSystemLogger, "no se establecio conexion con %s", tipo);
        //fileSystem_destruir(fileSystemConfig, fileSystemLogger);
        exit(-1);
    }
    log_info(fileSystemLogger, "se establecio conexion con %s", tipo);
}

int main(int argc, char* argv[]){

    fileSystemLogger = log_create(LOGS_FILESYSTEM, MODULO_FILESYSTEM, true, LOG_LEVEL_DEBUG);
    fileSystemConfig = fileSystem_config_crear(argv[1], fileSystemLogger);

    // Conexion Memoria, Cliente

    const int socketMemoria = conectar_a_servidor(fileSystem_config_obtener_ip_memoria(fileSystemConfig), fileSystem_config_obtener_puerto_memoria(fileSystemConfig));
    avisar_si_hay_error(socketMemoria, "Memoria");
    intentar_establecer_conexion(socketMemoria, "Memoria");

    // Conexion Kernel, Servidor

    int socketKernel = iniciar_servidor(NULL, fileSystem_config_obtener_puerto_escucha(fileSystemConfig));
    avisar_si_hay_error(socketKernel, "SERVIDOR DE KERNEL DEL FILESYSTEM");

}

