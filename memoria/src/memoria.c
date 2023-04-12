#include "../include/memoria_config.h"

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define LOGS_MEMORIA "../bin/memoria.log"
#define MODULO_MEMORIA "Memoria"

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

int handshake_filesystem(int socketFilesystem){
    uint8_t respuestaKERNEL = stream_recibir_header(socketFilesystem);
    stream_recibir_buffer_vacio(socketFilesystem);
    if (respuestaKERNEL != HANDSHAKE_filesystem) {
        log_error(memoriaLogger, "error al intentar establecer HANDSHAKE inicial con kernel");
        log_destroy(memoriaLogger);
        return -1;
    }
    stream_enviar_buffer_vacio(socketFilesystem, HANDSHAKE_puede_continuar);
    log_info(memoriaLogger, "conexion con kernel establecida");
}

void avisar_si_hay_error(int socket, char* tipo){
    if (socket == -1) {
        log_error(memoriaLogger, "no se pudo establecer conexion inicial con %s", tipo);
        exit(-1);
    }
}

int main(){
    // inicializa servidor de escucha 
    int socketESCUCHA = iniciar_servidor(NULL, memoria_config_obtener_puerto_escucha(memoriaConfig));
    avisar_si_hay_error(socketESCUCHA, "SERVIDOR DE ESCUCHA FILESYSTEM KERNEL CPU");

    int handshake = handshake_filesystem(socketESCUCHA);
    if (handshake == -1){
        log_error(memoriaLogger, "Falló el Handshake con el Filesystem");
        exit (-1);
    }
    // ver como recibir varias conexiones (hilos o select()/poll())
}

