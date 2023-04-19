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
        log_error(fileSystemLogger, "error al intentar establecer HANDSHAKE inicial con kernelSOCKET");
        log_destroy(fileSystemLogger);
        exit(-1);
    }
    stream_enviar_buffer_vacio(socketKernel, HANDSHAKE_puede_continuar);
    log_info(fileSystemLogger, "conexion con KERNEL establecida");
}

int main(int argc, char* argv[]){

    fileSystemLogger = log_create(LOGS_FILESYSTEM, MODULO_FILESYSTEM, true, LOG_LEVEL_DEBUG);

    fileSystemConfig = fileSystem_config_crear(argv[1], fileSystemLogger);


    // iniciar conexion con MEMORIA
    const int socketMEMORIA = conectar_a_servidor(fileSystem_config_obtener_ip_memoria(fileSystemConfig), fileSystem_config_obtener_puerto_memoria(fileSystemConfig));
    if (socketMEMORIA == -1) {
        log_error(fileSystemLogger, "Error al intentar conectar con FILESYSTEM");
        fileSystem_destruir(fileSystemConfig, fileSystemLogger);
        exit(-1);
    }
    fileSystem_config_setear_socket_memoria(fileSystemConfig, socketMEMORIA);

    stream_enviar_buffer_vacio(socketMEMORIA, HANDSHAKE_memoria);
    
    uint8_t MEMORIARespuesta = stream_recibir_header(socketMEMORIA);
    stream_recibir_buffer_vacio(socketMEMORIA);
    if (MEMORIARespuesta != HANDSHAKE_puede_continuar) {
        log_error(fileSystemLogger, "no se pudo conectar con FILESYSTEM");
        fileSystem_destruir(fileSystemConfig, fileSystemLogger);
        exit(-1);
    }
    log_info(fileSystemLogger, "se establecio conexion con FILESYSTEM");

    // acepta conexion con KERNEL

    int socketESCUCHA = iniciar_servidor(NULL, fileSystem_config_obtener_puerto_escucha(fileSystemConfig));
       
    struct sockaddr cliente = {0};
    socklen_t len = sizeof(cliente);

    avisar_si_hay_error(socketESCUCHA, "SERVIDOR DE ESCUCHA PARA KERNEL");

    log_info(fileSystemLogger, "ESPERANDO CLIENTES");
    int socketKERNEL = accept(socketESCUCHA, &cliente, &len);
    handshake_kernel(socketKERNEL);
    

}

