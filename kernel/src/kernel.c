
#include "../include/kernel_config.h"
#include "../include/planificador.h"


#define LOGS_KERNEL "bin/kernel.log"
#define MODULO_KERNEL "Kernel"
#define NUMERO_DE_ARGUMENTOS_NECESARIOS 2 

extern t_log* kernelLogger;
extern t_kernel_config* kernelConfig;

int socketFILESYSTEM;

int socketMEMORIA;

int cantidadDeSegmentos;

 void kernel_acepta_conexion(int socketEscucha) {
    struct sockaddr cliente = {0};
    socklen_t len = sizeof(cliente);
    log_info(kernelLogger, "Listo para escuchar nuevas conexiones");
    for (;;) {
        int clienteAceptado = accept(socketEscucha, &cliente, &len);
        if (clienteAceptado > -1) {
            int* socketCliente = malloc(sizeof(*socketCliente));
            *socketCliente = clienteAceptado;
            pthread_t threadSuscripcion;
            pthread_create(&threadSuscripcion, NULL, encolar_en_new_nuevo_pcb_entrante, (void*)socketCliente);
            pthread_detach(threadSuscripcion);
        } else {
            log_error(kernelLogger, "Error al aceptar conexión: %s", strerror(errno));
        }
    }
}

void conectar_con_file_system(int* socket, uint8_t handshake)
{
    char* servidor = handshake == HANDSHAKE_kernel ? "FILESYSTEM-PETICIONES" : "FILESYSTEM-DESBLOQUEOS";
    log_info(kernelLogger, "Conectando con %s...", servidor);
    *socket = conectar_a_servidor(kernel_config_obtener_ip_filesystem(kernelConfig), kernel_config_obtener_puerto_filesystem(kernelConfig));
    if (*socket == -1) {
        log_error(kernelLogger, "Error al intentar conectar con %s", servidor);
        kernel_destruir(kernelConfig, kernelLogger);
        exit(-1);
    }

    stream_enviar_buffer_vacio(*socket, handshake);
    
    uint8_t FILESYSTEMRespuesta = stream_recibir_header(*socket);
    stream_recibir_buffer_vacio(*socket);
    if (FILESYSTEMRespuesta != HANDSHAKE_puede_continuar) {
        log_error(kernelLogger, "no se pudo conectar con %s", servidor);
        kernel_destruir(kernelConfig, kernelLogger);
        exit(-1);
    }
    log_info(kernelLogger, "se establecio conexion con %s", servidor);
}

void kernel_destruir(t_kernel_config* kernelConfig, t_log* kernelLog) {
    kernel_config_destruir(kernelConfig);
    log_destroy(kernelLog);
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
        log_destroy(kernelLogger);
        exit(-1);
    }
    log_info(kernelLogger, "se establecio conexion con %s", tipo);
}

int recibir_cant_segmentos_de_memoria(){

    uint8_t MEMORIARespuesta = stream_recibir_header(socketMEMORIA);
    t_buffer* buffer = buffer_crear();
    stream_recibir_buffer(socketMEMORIA,buffer);
    if (MEMORIARespuesta != HEADER_cantidad_seg_enviada) {
        log_error(kernelLogger, "error al recibir cantidad de segmentos de memoria");
        kernel_destruir(kernelConfig, kernelLogger);
        exit(-1);
    }
    log_info(kernelLogger, "se recibio la cantidad de segmentos de memoria");
    int segmentos;
    buffer_desempaquetar(buffer,&segmentos,sizeof(segmentos));
    
    buffer_destruir(buffer);
    return segmentos;
}


int main(int argc, char* argv[]){
    kernelLogger = log_create(LOGS_KERNEL, MODULO_KERNEL, true, LOG_LEVEL_DEBUG);

    kernelConfig = kernel_config_crear(argv[1], kernelLogger);
    

    if (argc != NUMERO_DE_ARGUMENTOS_NECESARIOS) {
        log_error(kernelLogger, "Cantidad de argumentos inválida.\nArgumentos: <tamañoProceso> <pathInstrucciones>");
        kernel_destruir(kernelConfig, kernelLogger);
        return -1;
    }
    


    // conexion con CPU
    const int socketCPU = conectar_a_servidor(kernel_config_obtener_ip_cpu(kernelConfig), kernel_config_obtener_puerto_cpu(kernelConfig));
    if (socketCPU == -1) {
        log_error(kernelLogger, "Error al intentar conectar con CPU");
        kernel_destruir(kernelConfig, kernelLogger);
        exit(-1);
    }
    kernel_config_setear_socket_cpu(kernelConfig, socketCPU);

    stream_enviar_buffer_vacio(socketCPU, HANDSHAKE_kernel);
    
    uint8_t CPURespuesta = stream_recibir_header(socketCPU);
    stream_recibir_buffer_vacio(socketCPU);
    if (CPURespuesta != HANDSHAKE_puede_continuar) {
        log_error(kernelLogger, "no se pudo conectar con CPU");
        kernel_destruir(kernelConfig, kernelLogger);
        exit(-1);
    }
    log_info(kernelLogger, "se establecio conexion con CPU");


    // conexion con MEMORIA
    socketMEMORIA = conectar_a_servidor(kernel_config_obtener_ip_memoria(kernelConfig), kernel_config_obtener_puerto_memoria(kernelConfig));
    if (socketMEMORIA == -1) {
        log_error(kernelLogger, "Error al intentar conectar con MEMORIA");
        kernel_destruir(kernelConfig, kernelLogger);
        exit(-1);
    }
    kernel_config_setear_socket_memoria(kernelConfig, socketMEMORIA);

    stream_enviar_buffer_vacio(socketMEMORIA, HANDSHAKE_kernel);
    
    uint8_t MEMORIARespuesta = stream_recibir_header(socketMEMORIA);
    stream_recibir_buffer_vacio(socketMEMORIA);
    if (MEMORIARespuesta != HANDSHAKE_puede_continuar) {
        log_error(kernelLogger, "no se pudo conectar con MEMORIA");
        kernel_destruir(kernelConfig, kernelLogger);
        exit(-1);
    }
    log_info(kernelLogger, "se establecio conexion con MEMORIA");

    cantidadDeSegmentos = recibir_cant_segmentos_de_memoria();



    // conexion con FILESYSTEM

    conectar_con_file_system(&socketFILESYSTEM, HANDSHAKE_kernel);
    kernel_config_setear_socket_filesystem_peticiones(kernelConfig, socketFILESYSTEM);

    int socketFilesystemDesbloqueos;
    conectar_con_file_system(&socketFilesystemDesbloqueos, HANDSHAKE_kernel_desbloqueos);
    kernel_config_setear_socket_filesystem_desbloqueos(kernelConfig, socketFilesystemDesbloqueos);

    

    // inicializa servidor de instancias CONSOLA /// ARREGLAR 
    int socketESCUCHA = iniciar_servidor("0.0.0.0", kernel_config_obtener_puerto_escucha(kernelConfig));
    avisar_si_hay_error(socketESCUCHA, "SERVIDOR DE INSTANCIAS CONSOLA");


    // inicializar 

    iniciar_planificadores();
    // TODO

    kernel_acepta_conexion(socketESCUCHA);

}