#include "../include/comunicacionMemoria.h"


// mem_adapter_
void memoria_finalizar_proceso(t_pcb* pcbAFinalizar, t_kernel_config* kernelConfig, t_log* kernelLogger) {
   /* 
    VER COMO AVISAR A MEMORIA CON SEGM
   
    pthread_mutex_lock(&mutexSocketMemoria);
    uint32_t tablaAFinalizar = pcb_get_tabla_pagina_primer_nivel(pcbAFinalizar);

    t_buffer* bufferPcbAFinalizar = buffer_create();
    buffer_pack(bufferPcbAFinalizar, &tablaAFinalizar, sizeof(tablaAFinalizar));

    stream_send_buffer(kernel_config_get_socket_memoria(kernelConfig), HEADER_proceso_terminado, bufferPcbAFinalizar);
    buffer_destroy(bufferPcbAFinalizar);

    uint8_t memoriaResponse = stream_recv_header(kernel_config_get_socket_memoria(kernelConfig));
    stream_recv_empty_buffer(kernel_config_get_socket_memoria(kernelConfig));
    if (memoriaResponse == HANDSHAKE_ok_continue) {
        log_info(kernelLogger, "Proceso %d finalizado correctamente en Memoria", pcb_get_pid(pcbAFinalizar));
    } else {
        log_error(kernelLogger, "Error al finalizar proceso en Memoria");
        exit(-1);
    }
    pthread_mutex_unlock(&mutexSocketMemoria);*/ 
}