#include "../include/memoria_config.h"

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define LOGS_MEMORIA "bin/memoria.log"
#define MODULO_MEMORIA "Memoria"
#define NUMBER_OF_ARGS_REQUIRED 2 

extern t_log* memoriaLogger;
extern t_memoria_config* memoriaConfig;

/*     Hacer algo parecido o decidir que usar (hilos/select())
static void kernel_acepta_conexion(int socketEscucha) {
    struct sockaddr cliente = {0};
    socklen_t len = sizeof(cliente);
    log_info(kernelLogger, "listo para escuchar nuevas conexiones %d", socketEscucha);
    for (;;) {
        int clienteAceptado = accept(socketEscucha, &cliente, &len);
        if (clienteAceptado > -1) {
            int* socketCliente = malloc(sizeof(*socketCliente));
            *socketCliente = clienteAceptado;
//        crear hilo por cada conexion entrante???
        } else {
            log_error(kernelLogger, "error al aceptar conexión: %s", strerror(errno));
        }
    }
}
*/

void handshake_filesystem(int socketFilesystem){
    uint8_t respuestaFILESYSTEM = stream_recibir_header(socketFilesystem);
    stream_recibir_buffer_vacio(socketFilesystem);
    if (respuestaFILESYSTEM != HANDSHAKE_filesystem) {
        log_error(memoriaLogger, "error al intentar establecer HANDSHAKE inicial con FILESYSTEM");
        log_destroy(memoriaLogger);
        exit(-1);
    }
    stream_enviar_buffer_vacio(socketFilesystem, HANDSHAKE_puede_continuar);
    log_info(memoriaLogger, "conexion con FILESYSTEM establecida");
}

void handshake_cpu(int socketCPU){
    uint8_t respuestaCPU = stream_recibir_header(socketCPU);
    stream_recibir_buffer_vacio(socketCPU);
    if (respuestaCPU != HANDSHAKE_cpu) {
        log_error(memoriaLogger, "error al intentar establecer HANDSHAKE inicial con socketCPU");
        log_destroy(memoriaLogger);
        exit(-1);
    }
    stream_enviar_buffer_vacio(socketCPU, HANDSHAKE_puede_continuar);
    log_info(memoriaLogger, "conexion con CPU establecida");
}



void handshake_kernel(int socketKERNEL){
    uint8_t respuestaKERNEL = stream_recibir_header(socketKERNEL);
    stream_recibir_buffer_vacio(socketKERNEL);
    if (respuestaKERNEL != HANDSHAKE_kernel) {
        log_error(memoriaLogger, "error al intentar establecer HANDSHAKE inicial con socketKERNEL");
        log_destroy(memoriaLogger);
        exit(-1);
    }
    stream_enviar_buffer_vacio(socketKERNEL, HANDSHAKE_puede_continuar);
    log_info(memoriaLogger, "conexion con KERNEL establecida");
}
/* para probar cuando sepamos que ande: generalizacion de las 2 funciones de arriba 

void handshake(int socket, (enum <unnamed>) HANDSHAKE_MODULO, char* tipo, t_log logger ){
    uint8_t respuesta = stream_recibir_header(socket);
    stream_recibir_buffer_vacio(socket);
    if (respuestaCPU != HANDSHAKE_MODULO) {
        log_error(logger, "error al intentar establecer HANDSHAKE inicial con %s", tipo);
        log_destroy(logger);
        exit(-1);
    }
    stream_enviar_buffer_vacio(socketCPU, HANDSHAKE_puede_continuar);
    log_info(memoriaLogger, "conexion con CPU establecida");
    
}
*/
void avisar_si_hay_error(int socket, char* tipo){
    if (socket == -1) {
        log_error(memoriaLogger, "no se pudo establecer conexion inicial con %s", tipo);
        exit(-1);
    }
}

int main(int argc, char* argv[]){

    memoriaLogger = log_create(LOGS_MEMORIA, MODULO_MEMORIA, true, LOG_LEVEL_INFO);
    if (argc != NUMBER_OF_ARGS_REQUIRED) {
        log_error(memoriaLogger, "Cantidad de argumentos inválida.\nArgumentos: <configPath>");
        log_destroy(memoriaLogger);
        return -1;
    }
    memoriaConfig = memoria_config_crear(argv[1], memoriaLogger);

    // inicializa servidor de escucha 
    int socketESCUCHA = iniciar_servidor(NULL, memoria_config_obtener_puerto_escucha(memoriaConfig));
    
    struct sockaddr cliente = {0};
    socklen_t len = sizeof(cliente);

    avisar_si_hay_error(socketESCUCHA, "SERVIDOR DE ESCUCHA FILESYSTEM KERNEL CPU");

    log_info(memoriaLogger, "ESPERANDO CLIENTES");

    // acepta conexion con CPU
    int socketCPU = accept(socketESCUCHA, &cliente, &len);

    handshake_cpu(socketCPU);

    // acepta conexion con FILESYSTEM .... ESTO es lo que hay que arreglar (del lado de FILESYSTEM)

    int socketFilesystem = accept(socketESCUCHA, &cliente, &len);

    handshake_filesystem(socketFilesystem);
    

    // acepta conexion con KERNEL

    int socketKernel = accept(socketESCUCHA, &cliente, &len);

    handshake_kernel(socketKernel);
    // ver como recibir varias conexiones (hilos o select()/poll())
}

