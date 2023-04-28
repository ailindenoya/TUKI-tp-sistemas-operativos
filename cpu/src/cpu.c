#include "../include/cpu_config.h"


#define LOGS_CPU "bin/cpu.log"
#define MODULO_CPU "CPU"
#define CONFIG_CPU "cfg/cpu_config.cfg"
#define NUMERO_DE_ARGUMENTOS_NECESARIOS 2


extern t_log* cpuLogger;
extern t_cpu_config* cpuConfig;
t_pcb* contexteto_ejecucion;


int main(int argc, char* argv[]) {
   
    cpuLogger = log_create(LOGS_CPU, MODULO_CPU, true, LOG_LEVEL_INFO);
    if (argc != NUMERO_DE_ARGUMENTOS_NECESARIOS) {
        log_error(cpuLogger, "Cantidad de argumentos inválida.\nArgumentos: <configPath>");
        log_destroy(cpuLogger);
        return -1;
    }    
    cpuConfig = cpu_config_crear(CONFIG_CPU, cpuLogger);

    // conexion con MEMORIA

    const int socketMEMORIA = conectar_a_servidor(cpu_config_obtener_ip_memoria(cpuConfig), cpu_config_obtener_puerto_memoria(cpuConfig));
    if (socketMEMORIA == -1) {
        log_error(cpuLogger, "no se establecio conexion con MEMORIA");
        log_destroy(cpuLogger);
        return -1;
    }

    cpu_config_setear_socket_memoria(cpuConfig,socketMEMORIA);

    stream_enviar_buffer_vacio(socketMEMORIA, HANDSHAKE_cpu);
    uint8_t memoriaRespuesta = stream_recibir_header(socketMEMORIA);
    t_buffer* bufferMemoria = buffer_crear();
    stream_recibir_buffer(cpu_config_obtener_socket_memoria(cpuConfig), bufferMemoria);

    if (memoriaRespuesta != HANDSHAKE_puede_continuar) {
        log_error(cpuLogger, "no se establecio conexion con MEMORIA");
        log_destroy(cpuLogger);
        return -1;
    }else{
        log_info(cpuLogger, "se establecio conexion con MEMORIA");
    }

    // aceptar conexion con KERNEL

    int socketKERNELESCUCHA= iniciar_servidor(cpu_config_obtener_ip_cpu(cpuConfig), cpu_config_obtener_puerto_escucha(cpuConfig));
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
    log_info(cpuLogger, "se establecio conexion con KERNEL");

    //recibir
}
