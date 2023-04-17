#include "../include/cpu_config.h"


#define LOGS_CPU "bin/cpu.log"
#define MODULO_CPU "CPU"
#define CONFIG_CPU "cfg/cpu_config.cfg"


extern t_log* cpuLogger;
extern t_cpu_config* cpuConfig;

int main(int argc, char* argv[]) {
    cpuLogger = log_create(LOGS_CPU, MODULO_CPU, true, LOG_LEVEL_INFO);
    log_info(cpuLogger, "hola :D");
    cpuConfig = cpu_config_crear(CONFIG_CPU, cpuLogger);

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
    stream_recibir_buffer(cpu_config_obtener_socket_memoria(cpuConfig), bufferMemoria);

    if (memoriaResponse != HANDSHAKE_puede_continuar) {
        log_error(cpuLogger, "no se establecio conexion con Memoria");
        log_destroy(cpuLogger);
        return -1;
    }    

    log_info(cpuLogger, "hice el handshake con memoria");

    // aceptar conexion con kernel
    int socketKERNELESCUCHA= iniciar_servidor(NULL, cpu_config_obtener_puerto_escucha(cpuConfig));
        struct sockaddr cliente = {0};
    socklen_t len = sizeof(cliente);

    int socketKERNEL = accept(socketKERNELESCUCHA, &cliente, &len);
    if (socketKERNEL == -1) {
        log_error(cpuLogger, "no se pudo establecer conexion inicial con KERNEL");
        log_destroy(cpuLogger);
        return -1;
    }
    cpu_config_setear_socket_kernel(cpuConfig, socketKERNEL);

    uint8_t respuestaKERNEL = stream_recibir_header(socketKERNEL);
    stream_recibir_buffer_vacio(socketKERNEL);
    if (respuestaKERNEL != HANDSHAKE_kernel) {
        log_error(cpuLogger, "error al intentar establecer HANDSHAKE inicial con kernel");
        log_destroy(cpuLogger);
        return -1;
    }
    stream_enviar_buffer_vacio(socketKERNEL, HANDSHAKE_puede_continuar);
    log_info(cpuLogger, "conexion con kernel establecida");

}
