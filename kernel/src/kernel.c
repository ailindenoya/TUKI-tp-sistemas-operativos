
#include "../include/kernel_config.h"


#define LOGS_KERNEL "../bin/kernel.log"
#define MODULO_KERNEL "Kernel"

extern t_log* kernelLogger;
extern t_kernel_config* kernelConfig;

static void kernel_acepta_conexion(int socketEscucha);
static void kernel_destruir(t_kernel_config* kernelConfig, t_log* kernelLogger);

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

static void kernel_destruir(t_kernel_config* kernelConfig, t_log* kernelLogger) {
    kernel_config_destruir(kernelConfig);
    log_destroy(kernelLogger);
}

void avisar_si_hay_error(int socket, char* tipo){
    if (socket == -1) {
        log_error(kernelLogger, "no se pudo establecer conexion inicial con %s", tipo);
        kernel_destruir(kernelConfig, kernelLogger);
        exit(-1);
    }
}

void intentar_establecer_conexion(int socket, char* tipo){
    stream_enviar_buffer_vacio(socket, HANDSHAKE_interrupcion);
    uint8_t respuestaInterrupcion = stream_recibir_header(socket);
    stream_recibir_buffer_vacio(respuestaInterrupcion);
    if (respuestaInterrupcion != HANDSHAKE_puede_continuar) {
        log_error(kernelLogger, "no se establecio conexion con %s", tipo);
        kernel_destruir(kernelConfig, kernelLogger);
        exit(-1);
    }
    log_info(kernelLogger, "se establecio conexion con %s", tipo);
}

int main(int argc, char* argv[]){
    kernelLogger = log_create(LOGS_KERNEL, MODULO_KERNEL, true, LOG_LEVEL_DEBUG);

    kernelConfig = kernel_config_crear(argv[1], kernelLogger);

    // conexion con CPU

    const int socketCPU = conectar_a_servidor(kernel_config_obtener_ip_cpu(kernelConfig), kernel_config_obtener_puerto_cpu(kernelConfig));
    avisar_si_hay_error(socketCPU, "CPU");
    intentar_establecer_conexion(socketCPU, "CPU");

    // conexion con MEMORIA

    const int socketMEMORIA = conectar_a_servidor(kernel_config_obtener_ip_memoria(kernelConfig), kernel_config_obtener_puerto_memoria(kernelConfig));
    avisar_si_hay_error(socketMEMORIA, "MEMORIA");
    intentar_establecer_conexion(socketMEMORIA, "MEMORIA");


    // conexion con FILESYSTEM

    const int socketFILESYSTEM = conectar_a_servidor(kernel_config_obtener_ip_filesystem(kernelConfig), kernel_config_obtener_puerto_filesystem(kernelConfig));
    avisar_si_hay_error(socketFILESYSTEM, "FILESYSTEM");
    intentar_establecer_conexion(socketFILESYSTEM, "FILESYSTEM");

    // inicializa servidor de instancias CONSOLA
    int socketESCUCHA = iniciar_servidor(kernel_config_obtener_puerto_escucha(kernelConfig), kernel_config_obtener_puerto_escucha(kernelConfig));
    avisar_si_hay_error(socketESCUCHA, "SERVIDOR DE INSTANCIAS CONSOLA");


    // inicializar estructuras 
    // TODO

    kernel_acepta_conexion(socketESCUCHA);

}