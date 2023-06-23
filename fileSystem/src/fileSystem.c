#include "../include/fileSystem_config.h"
#include "../include/bitmap.h"
#include "../include/comunicacionKernelYMemoria.h"


#define LOGS_FILESYSTEM "bin/fileSystem.log"
#define MODULO_FILESYSTEM "fileSystem"
#define NUMERO_DE_ARGUMENTOS_NECESARIOS 3

extern t_log* fileSystemLogger;
extern t_fileSystem_config* fileSystemConfig;
extern t_superbloque_config* superbloqueConfig;
t_list* listaFCBsAbiertos;
int socketKERNEL;

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
    log_info(fileSystemLogger, "se establecio conexion con KERNEL");
}

int main(int argc, char* argv[]){

    fileSystemLogger = log_create(LOGS_FILESYSTEM, MODULO_FILESYSTEM, true, LOG_LEVEL_DEBUG);

    fileSystemConfig = fileSystem_config_crear(argv[1], fileSystemLogger);
    superbloqueConfig = superbloque_config_crear(argv[2], fileSystemLogger);

    if (argc != NUMERO_DE_ARGUMENTOS_NECESARIOS) {
        log_error(fileSystemLogger, "Cantidad de argumentos inválida.\nArgumentos: <tamañoProceso> <pathInstrucciones>");
        fileSystem_destruir(fileSystemConfig,fileSystemLogger );
        return -1;
    }    
    // conexion con MEMORIA

    const int socketMEMORIA = conectar_a_servidor(fileSystem_config_obtener_ip_memoria(fileSystemConfig), fileSystem_config_obtener_puerto_memoria(fileSystemConfig));
    if (socketMEMORIA == -1) {
        log_error(fileSystemLogger, "Error al intentar conectar con MEMORIA");
        fileSystem_destruir(fileSystemConfig, fileSystemLogger);
        exit(-1);
    }
    
    fileSystem_config_setear_socket_memoria(fileSystemConfig, socketMEMORIA);

    stream_enviar_buffer_vacio(socketMEMORIA, HANDSHAKE_filesystem);
    uint8_t MEMORIARespuesta = stream_recibir_header(socketMEMORIA);
    stream_recibir_buffer_vacio(socketMEMORIA);

    if (MEMORIARespuesta != HANDSHAKE_puede_continuar) {
        log_error(fileSystemLogger, "no se pudo conectar con MEMORIA");
        fileSystem_destruir(fileSystemConfig, fileSystemLogger);
        exit(-1);
    }
    log_info(fileSystemLogger, "se establecio conexion con MEMORIA");

    t_bitarray* bitmap = cargarBitMap();
    listaFCBsAbiertos = list_create();

    // acepta conexion con KERNEL

    int socketESCUCHA = iniciar_servidor("0.0.0.0", fileSystem_config_obtener_puerto_escucha(fileSystemConfig));
       
    struct sockaddr cliente = {0};
    socklen_t len = sizeof(cliente);

    avisar_si_hay_error(socketESCUCHA, "SERVIDOR DE ESCUCHA PARA KERNEL");

    log_info(fileSystemLogger, "ESPERANDO CLIENTES");
    socketKERNEL = accept(socketESCUCHA, &cliente, &len);
    handshake_kernel(socketKERNEL);
    
    atenderPeticionesDeKernel();

}

