
extern int cantidadDeSegmentos;
#include "../include/comunicacionCPU.h"



void kernel_enviar_pcb_a_cpu(t_pcb* pcbAEnviar, t_kernel_config* kernelConfig, t_log* kernelLogger, uint8_t header) {
    uint32_t pidAEnviar = pcb_obtener_pid(pcbAEnviar);
    uint32_t pcAEnviar = pcb_obtener_program_counter(pcbAEnviar);

    t_buffer* bufferPcbAEjecutar = buffer_crear();
    buffer_empaquetar(bufferPcbAEjecutar, &pidAEnviar, sizeof(pidAEnviar));
    buffer_empaquetar(bufferPcbAEjecutar, &pcAEnviar, sizeof(pcAEnviar));
    buffer_empaquetar_tabla_de_segmentos(bufferPcbAEjecutar, pcb_obtener_tabla_de_segmentos(pcbAEnviar),cantidadDeSegmentos);
    stream_enviar_buffer(kernel_config_obtener_socket_cpu(kernelConfig), header, bufferPcbAEjecutar);
    stream_enviar_buffer(kernel_config_obtener_socket_cpu(kernelConfig), HEADER_lista_de_instrucciones, pcb_obtener_buffer_de_instrucciones(pcbAEnviar));

    buffer_destruir(bufferPcbAEjecutar);
}


t_pcb* kernel_recibir_pcb_actualizado_de_cpu(t_pcb* pcbAActualizar, uint8_t cpuResponse, t_kernel_config* kernelConfig, t_log* kernelLogger) {
    uint32_t pidRecibido = 0;
    uint32_t programCounterActualizado = 0;
    uint32_t tiempoDeBloqActualizado = 0;

    t_buffer* bufferPcb = buffer_crear();
    stream_recibir_buffer(kernel_config_obtener_socket_cpu(kernelConfig), bufferPcb);
    buffer_desempaquetar(bufferPcb, &pidRecibido, sizeof(pidRecibido));
    buffer_desempaquetar(bufferPcb, &programCounterActualizado, sizeof(programCounterActualizado));
    
    if (pidRecibido == pcb_obtener_pid(pcbAActualizar)) { /// si es el mismo al que tenes en kernel
        if (cpuResponse == HEADER_proceso_bloqueado) {
            buffer_desempaquetar(bufferPcb, &tiempoDeBloqActualizado, sizeof(tiempoDeBloqActualizado)); // desempaqueta tiempo que se bloqueo
            pcb_setear_tiempo_bloqueo(pcbAActualizar, tiempoDeBloqActualizado);
            // solo para instruccion de io
        } // cpu le devuelve a kernel el contexto y el tiempo (nro al lado de IO o parametro) que se va a bloquear el proceso
        
        pcb_setear_program_counter(pcbAActualizar, programCounterActualizado);
    } else {
        log_error(kernelLogger, "Error al recibir PCB de CPU");
        exit(-1);
    }
    buffer_destruir(bufferPcb);
    return pcbAActualizar;
}