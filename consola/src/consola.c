#include "../include/consola_config.h"
#include "../include/consola_parser.h"




#define LOGS_CONSOLA "bin/consola.log"
#define MODULO_CONSOLA "Consola"
#define NUMERO_DE_ARGUMENTOS_NECESARIOS 3 


void consola_destruir(t_log *consolaLogger,t_consola_config *consolaConfig ){
    log_destroy(consolaLogger);
    consola_config_destruir(consolaConfig);
}

void consola_enviar_instrucciones_a_kernel(const char *pathInstrucciones, t_log *consolaLogger, int kernelSocket) {
    t_buffer *bufferDeInstrucciones = buffer_crear();
    consola_parsear_instrucciones(bufferDeInstrucciones, pathInstrucciones, consolaLogger);
    stream_enviar_buffer(kernelSocket, HEADER_lista_de_instrucciones, bufferDeInstrucciones);
    log_info(consolaLogger, "se envio lista de INSTRUCCIONES a KERNEL");
    buffer_destruir(bufferDeInstrucciones);
    return;
}

int main(int argc, char *argv[]) {

    t_log *consolaLogger = log_create(LOGS_CONSOLA, MODULO_CONSOLA, true, LOG_LEVEL_INFO);
    
    t_consola_config *consolaConfig = consola_crear_config(argv[1], consolaLogger);

    if (argc != NUMERO_DE_ARGUMENTOS_NECESARIOS) {
        log_error(consolaLogger, "Cantidad de argumentos inválida.\nArgumentos: <tamañoProceso> <pathInstrucciones>");
        consola_destruir(consolaLogger, consolaConfig);
        return -1;
    }
    
    // conexion con KERNEL

    const int kernelSocket = conectar_a_servidor(consola_config_obtener_kernel_IP(consolaConfig), consola_config_obtener_kernel_PUERTO(consolaConfig));
    if (kernelSocket == -1) {
        log_error(consolaLogger, "Consola no se pudo conectar con Kernel");
        consola_destruir(consolaLogger, consolaConfig);
        return -1;
    }

    // envio de instrucciones a KERNEL

    t_buffer *buffer = buffer_crear();
    uint32_t tamanioProceso = 500;
    buffer_empaquetar(buffer, &tamanioProceso, sizeof(tamanioProceso));
    stream_enviar_buffer(kernelSocket, HANDSHAKE_consola, buffer);
    buffer_destruir(buffer);

    uint8_t kernelRespuesta = stream_recibir_header(kernelSocket);
    stream_recibir_buffer_vacio(kernelSocket);
    if (kernelRespuesta != HANDSHAKE_puede_continuar) {
        log_error(consolaLogger, "no se pudo establecer HANDSHAKE INICIAL con KERNEL");
        consola_destruir(consolaLogger, consolaConfig);
        return -1;
    }

    const char *pathInstrucciones = argv[2];
    consola_enviar_instrucciones_a_kernel(pathInstrucciones, consolaLogger, kernelSocket);
    kernelRespuesta = stream_recibir_header(kernelSocket);
    if (kernelRespuesta != HEADER_pid) {
        log_error(consolaLogger, "Error al intentar recibir el PID");
        consola_destruir(consolaLogger, consolaConfig);
        return -1;
    }

    uint32_t procesoID = 0;
    t_buffer *bufferPID = buffer_crear();
    stream_recibir_buffer(kernelSocket, bufferPID);
    buffer_desempaquetar(bufferPID, &procesoID, sizeof(procesoID));
    buffer_destruir(bufferPID);

    kernelRespuesta = stream_recibir_header(kernelSocket);
    stream_recibir_buffer_vacio(kernelSocket);
    if (kernelRespuesta != HEADER_proceso_terminado) {
        log_error(consolaLogger, "Error al intentar finalizar consola con <ID %d>", procesoID);
        consola_destruir(consolaLogger, consolaConfig);
        return -1;
    } else {
        log_info(consolaLogger, "Finalización de consola %d", procesoID);
    }

    // limpieza

    consola_config_destruir(consolaConfig);
    log_destroy(consolaLogger);
    return 0;

}





