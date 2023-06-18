#include "../include/memoria_config.h"

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define LOGS_MEMORIA "bin/memoria.log"
#define MODULO_MEMORIA "Memoria"
#define NUMERO_DE_ARGUMENTOS_NECESARIOS 2 

extern t_log* memoriaLogger;
extern t_memoria_config* memoriaConfig;

int socketKERNEL;

struct proceso{
    int pid; 
    int * tablaDeSegmentos;
};

typedef proceso proceso; 
struct hueco_libre{
    int direccion;     
    int tamanio; 
};

typedef hueco_libre hueco_libre;


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

void handshake_kernel(){
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

void avisar_si_hay_error(int socket, char* tipo){
    if (socket == -1) {
        log_error(memoriaLogger, "no se pudo establecer conexion inicial con %s", tipo);
        exit(-1);
    }
}

void enviar_cant_segmentos_a_kernel(){
    t_buffer *buffer = buffer_crear();
    int cantidadDeSegmentos = memoria_config_obtener_cantidad_de_segmentos(memoriaConfig);
    buffer_empaquetar(buffer,&cantidadDeSegmentos , sizeof(cantidadDeSegmentos));
    stream_enviar_buffer(socketKERNEL, HEADER_cantidad_seg_enviada, buffer);
    buffer_destruir(buffer);
    log_info(memoriaLogger, "Se envio la cantidad de segmentos a Kernel");
}

void crear_segmento(int pid, int idSegmento, int tamanioSegmento){


}

void eliminar_segmento(int pid, int idSegmento ){

}

void recibir_de_kernel(){

    t_buffer * buffer = buffer_crear();
    
    for(; ;){
    uint8_t headerRecibido = stream_recibir_header(socketKERNEL);
    stream_recibir_buffer(socketKERNEL,buffer);
        switch (headerRecibido)
        {
        case HEADER_create_segment:
            int pid, idSegmento, tamanio;
            buffer_desempaquetar(buffer, &pid, sizeof(pid));
            buffer_desempaquetar(buffer, &idSegmento, sizeof(idSegmento));  
            buffer_desempaquetar(buffer, &tamanio,sizeof(tamanio));
            crear_segmento(pid,idSegmento, tamanio);
            break;
        case HEADER_delete_segment:
            int pid, idSegmento; 
            buffer_desempaquetar(buffer, &pid, sizeof(pid));
            buffer_desempaquetar(buffer,idSegmento,sizeof(idSegmento));
            eliminar_segmento(pid, idSegmento); 
            break;
        default:
            log_error(memoriaLogger, "Error al recibir mensaje de KERNEL");
            break;
        }

    }
    
}



int main(int argc, char* argv[]){


    memoriaLogger = log_create(LOGS_MEMORIA, MODULO_MEMORIA, true, LOG_LEVEL_INFO);
    if (argc != NUMERO_DE_ARGUMENTOS_NECESARIOS) {
        log_error(memoriaLogger, "Cantidad de argumentos inválida.\nArgumentos: <configPath>");
        log_destroy(memoriaLogger);
        return -1;
    }
    memoriaConfig = memoria_config_crear(argv[1], memoriaLogger);


    // inicializa servidor de escucha 
    
    int socketESCUCHA = iniciar_servidor("0.0.0.0", memoria_config_obtener_puerto_escucha(memoriaConfig));
    
    struct sockaddr cliente = {0};
    socklen_t len = sizeof(cliente);

    avisar_si_hay_error(socketESCUCHA, "SERVIDOR DE ESCUCHA FILESYSTEM KERNEL CPU");

    log_info(memoriaLogger, "ESPERANDO CLIENTES");

    // acepta conexion con CPU

    int socketCPU = accept(socketESCUCHA, &cliente, &len);

    handshake_cpu(socketCPU);

    // acepta conexion con FILESYSTEM 

    int socketFilesystem = accept(socketESCUCHA, &cliente, &len);

    handshake_filesystem(socketFilesystem);

    // acepta conexion con KERNEL

    socketKERNEL= accept(socketESCUCHA, &cliente, &len);

    handshake_kernel();

    enviar_cant_segmentos_a_kernel();
    
    void* bloque_de_memoria = malloc(memoria_config_obtener_tamanio_memoria(memoriaConfig));

    t_list* listaDeHuecosLibres = list_create();

    t_list* listaDeProcesos = list_create(); // al crear pcb
    pthread_t hiloDeProcesos;
    pthread_create(&hiloDeProcesos, NULL, (void*)recibir_de_kernel_proceso_agregado, NULL);


}

