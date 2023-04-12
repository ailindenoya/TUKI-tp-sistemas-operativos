#include "../cpu/include/cpu_config.h"


#include "../../utils/include/stream.h"
#define LOGS_CPU "bin/cpu.log"
#define MODULO_CPU "CPU"
#define CONFIG_CPU "cfg/cpu_config.cfg"


extern t_log* cpuLogger;
extern t_cpu_config* cpuConfig;

int main(int argc, char* argv[]) {
    cpuLogger = log_create(LOGS_CPU, MODULO_CPU, true, LOG_LEVEL_INFO);
    cpuConfig = cpu_config_create(argv[1], cpuLogger);

    // conexion con MEMORIA

    const int socketMEMORIA = conectar_a_servidor(cpu_config_obtener_ip_memoria(cpuConfig), cpu_config_obtener_puerto_memoria(cpuConfig));
    if (socketMEMORIA == -1) {
        log_error(cpuLogger, "no se establecio conexion con MEMORIA");
        log_destroy(cpuLogger);
        return -1;
    }

    stream_enviar_buffer_vacio(socketMEMORIA, HANDSHAKE_cpu);
    uint8_t memoriaResponse = stream_recibir_header(socketMEMORIA);
    t_buffer* bufferMemoria = buffer_crear();
    stream_recibir_buffer(cpu_config_setear_socket_memoria(cpuConfig), bufferMemoria);
    if (memoriaResponse != HANDSHAKE_puede_continuar) {
        log_error(cpuLogger, "no se establecio conexion con Memoria");
        log_destroy(cpuLogger);
        return -1;
    }    

    // aceptar conexion con kernel
    int socketKERNELESCUCHA= iniciar_servidor(cpu_config_obtener_ip_cpu(cpuConfig), cpu_config_obtener_puerto_escucha(cpuConfig));
        struct sockaddr cliente = {0};
    socklen_t len = sizeof(cliente);

    // Conexión con Kernel 
    int socketKERNEL = accept(socketKERNELESCUCHA, &cliente, &len);
    if (socketKERNEL == -1) {
        log_error(cpuLogger, "no se pudo establecer conexion inicial con KERNEL");
        log_destroy(cpuLogger);
        return -1;
    }
    cpu_config_set_socket_dispatch(cpuConfig, socketKERNEL);

    uint8_t kernelDispatchResponse = stream_recv_header(socketKERNEL);
    stream_recv_empty_buffer(socketKERNEL);
    if (kernelDispatchResponse != HANDSHAKE_dispatch) {
        log_error(cpuLogger, "Error al intentar establecer Handshake inicial con módulo Kernel por canal Dispatch");
        log_destroy(cpuLogger);
        return -1;
    }
    stream_send_empty_buffer(socketKERNEL, HANDSHAKE_puede_continuar);
    log_info(cpuLogger, "Conexión con Kernel por canal Dispatch establecida");

}