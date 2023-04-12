#include <errno.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/socket.h>

#include "../../utils/include/flagsParaCommons.h"
#include "../../utils/include/conexiones.h"
#include "../include/kernel_config.h"
#include "../../utils/include/stream.h"


#define LOGS_KERNEL "bin/kernel.log"
#define MODULO_KERNEL "Kernel"

extern t_log* kernelLogger;
extern t_kernel_config* kernelConfig;


static void kernel_aceptar_conexiones(int socketEscucha) {
    struct sockaddr cliente = {0};
    socklen_t len = sizeof(cliente);
    log_info(kernelLogger, "A la escucha de nuevas conexiones en puerto %d", socketEscucha);
    for (;;) {
        int clienteAceptado = accept(socketEscucha, &cliente, &len);
        if (clienteAceptado > -1) {
            int* socketCliente = malloc(sizeof(*socketCliente));
            *socketCliente = clienteAceptado;
        // crear hilo por cada conexion?
        } else {
            log_error(kernelLogger, "Error al aceptar conexión: %s", strerror(errno));
        }
    }
}

int main(int argc, char* argv[]){

    kernelLogger = log_create(LOGS_KERNEL, MODULO_KERNEL, true, LOG_LEVEL_DEBUG);

    kernelConfig = kernel_config_create(argv[1], kernelLogger);

    // Conexión con CPU en canal Dispatch
    const int socketCPUDispatch = conectar_a_servidor(kernel_config_get_ip_cpu(kernelConfig), kernel_config_get_puerto_cpu_dispatch(kernelConfig));
    if (socketCPUDispatch == -1) {
        log_error(kernelLogger, "Error al intentar establecer conexión inicial módulo CPU por canal Dispatch");
        __kernel_destroy(kernelConfig, kernelLogger);
        exit(-1);
    }
    kernel_config_set_socket_dispatch_cpu(kernelConfig, socketCPUDispatch);

    
}

