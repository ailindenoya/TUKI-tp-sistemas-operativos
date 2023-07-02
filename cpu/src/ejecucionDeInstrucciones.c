#include "../include/ejecucionDeInstrucciones.h"
#include "../include/mmu.h"
#include "../../utils/include/funcionesDeRegistrosDeCpu.h"
extern int cantidadDeSegmentos; 
extern t_log* cpuLogger;
extern t_cpu_config* cpuConfig;



registros registrosDeCpu;


t_instruccion* cpu_fetch_instruccion(t_contexto* contexto) {
    t_list* listaInstrucciones = contexto_obtener_instrucciones(contexto);
    uint32_t programCounter = contexto_obtener_program_counter(contexto);
    t_instruccion* instruccionSig = list_get(listaInstrucciones, programCounter);
    log_info(cpuLogger, "FETCH INSTRUCCION: PCB <ID %d>", contexto_obtener_pid(contexto));
    return instruccionSig;
}

void copiarStringAVector(char* string, char* vector, int tamanioDeRegistro) {
    for(int i = 0; i < tamanioDeRegistro; i++)
        vector[i] = string[i];
}

void empaquetar_contexto_para_kernel(t_buffer* buffer,uint32_t programCounterActualizado, t_contexto* contexto){
    uint32_t pid = contexto_obtener_pid(contexto);
    registros* registro = contexto_obtener_registros(contexto);
    buffer_empaquetar(buffer, &pid, sizeof(pid));
    buffer_empaquetar(buffer, &programCounterActualizado, sizeof(programCounterActualizado));
    buffer_empaquetar_registros(buffer,registro);
    buffer_empaquetar_tabla_de_segmentos(buffer, contexto_obtener_tabla_de_segmentos(contexto), cantidadDeSegmentos);
}

void ejecutar_SET(t_contexto* contexto, char* reg, char* param) {
    
    log_info(cpuLogger, "PID: %d - Ejecutando: SET ", contexto_obtener_pid(contexto));
    uint32_t retardo = (cpu_config_obtener_retardo_instruccion(cpuConfig))/1000;
    sleep(retardo);

    if(strcmp(reg,"AX") == 0){
        copiarStringAVector(param, registrosDeCpu.AX, 4);
    }else if (strcmp(reg,"BX") == 0){
        copiarStringAVector(param, registrosDeCpu.BX, 4);
    }else if (strcmp(reg,"CX") == 0){
        copiarStringAVector(param, registrosDeCpu.CX, 4);
    }else if (strcmp(reg,"DX") == 0){
        copiarStringAVector(param, registrosDeCpu.DX, 4);
    }else if (strcmp(reg,"EAX") == 0){
        copiarStringAVector(param, registrosDeCpu.EAX, 8);
    }else if (strcmp(reg,"EBX") == 0){
        copiarStringAVector(param, registrosDeCpu.EBX, 8);
    }else if (strcmp(reg,"ECX") == 0){
        copiarStringAVector(param, registrosDeCpu.ECX, 8);
    }else if (strcmp(reg,"EDX") == 0){
        copiarStringAVector(param, registrosDeCpu.EDX, 8);
    }else if (strcmp(reg,"RAX") == 0){
        copiarStringAVector(param, registrosDeCpu.RAX, 16);
    }else if (strcmp(reg,"RBX") == 0){
        copiarStringAVector(param, registrosDeCpu.RBX, 16);
    }else if (strcmp(reg,"RCX") == 0){
        copiarStringAVector(param, registrosDeCpu.RCX, 16);
    }else if (strcmp(reg,"RDX") == 0){
        copiarStringAVector(param, registrosDeCpu.RDX, 16);
    }else{
        log_info(cpuLogger, "error al ejecutar SET");
    }

}

void ejecutar_F_CLOSE(t_contexto* contexto,uint32_t programCounterActualizado){

}
void ejecutar_F_READ(t_contexto* contexto,uint32_t programCounterActualizado){

}
void ejecutar_F_WRITE(t_contexto* contexto,uint32_t programCounterActualizado){

}

void pedirleAMemoria(t_buffer* buffer, int cantidadDeBytes, char* valorDeMemoria){
    buffer_empaquetar(buffer, &cantidadDeBytes, sizeof(cantidadDeBytes));
    stream_enviar_buffer(cpu_config_obtener_socket_memoria(cpuConfig), HEADER_valor_de_memoria, buffer);

    int headerDeValorDeMemoria = stream_recibir_header(cpu_config_obtener_socket_memoria(cpuConfig));
    if (headerDeValorDeMemoria != HEADER_valor_de_memoria){
        log_error(cpuLogger, "error al recibir header de 'valor de memoria' de MEMORIA");
        exit(-1);
    }
    valorDeMemoria = realloc(valorDeMemoria,sizeof(*valorDeMemoria)*cantidadDeBytes);

    t_buffer* recibidoDeMemoria = buffer_crear();
    stream_recibir_buffer(cpu_config_obtener_socket_memoria(cpuConfig),recibidoDeMemoria); 
    log_info(cpuLogger, "CPU esta listo para empaquetar, con header recibido: %d", headerDeValorDeMemoria);
    buffer_desempaquetar(recibidoDeMemoria,valorDeMemoria, sizeof(*valorDeMemoria)*cantidadDeBytes); // ERROR : ACA NO HACE EL DESEMPAQUETADO
    log_info(cpuLogger, "desempaqueto bloque de memoria");
    buffer_destruir(recibidoDeMemoria); 
}

void ejecutar_MOV_IN(t_contexto* contexto,uint32_t programCounterActualizado, char* reg, char* dirLogica){
    uint32_t pid = contexto_obtener_pid(contexto);
    log_info(cpuLogger, "PID: %d - Ejecutando: MOV_IN ", pid);
    int direccionLogica = atoi(dirLogica);
    uint32_t nroSegmento = obtener_numero_de_segmento(direccionLogica);
    uint32_t offset = obtener_offset_de_segmento(direccionLogica);
    t_buffer* buffer = buffer_crear(); 
    if(offset < contexto_obtener_tabla_de_segmentos(contexto)[nroSegmento].tamanio){
        buffer_empaquetar(buffer, &pid, sizeof(pid));
        buffer_empaquetar(buffer,&nroSegmento, sizeof(nroSegmento));
        buffer_empaquetar(buffer,&offset, sizeof(offset));
        char* valorDeMemoria = malloc(sizeof(*valorDeMemoria));
        if (strcmp(reg, "AX") == 0){   
            log_info(cpuLogger, "llegue a AX");
            pedirleAMemoria(buffer, 4, valorDeMemoria);
            log_info(cpuLogger, "le pedi a memoria");
            copiarStringAVector(valorDeMemoria, registrosDeCpu.AX, 4);
            log_info(cpuLogger, "copie el string al vector");
        }else if (strcmp(reg, "BX") == 0){   
            pedirleAMemoria(buffer, 4, valorDeMemoria);
            copiarStringAVector(valorDeMemoria, registrosDeCpu.BX, 4);
        }else if (strcmp(reg, "CX") == 0){
            pedirleAMemoria(buffer, 4, valorDeMemoria);
            copiarStringAVector(valorDeMemoria, registrosDeCpu.CX, 4);
        }else if (strcmp(reg, "DX") == 0){
            pedirleAMemoria(buffer, 4, valorDeMemoria);    
            copiarStringAVector(valorDeMemoria, registrosDeCpu.DX, 4);
        }else if (strcmp(reg, "EAX") == 0){
            pedirleAMemoria(buffer, 8, valorDeMemoria);
            copiarStringAVector(valorDeMemoria, registrosDeCpu.EAX, 8);
        }else if (strcmp(reg, "EBX") == 0){
            pedirleAMemoria(buffer, 8, valorDeMemoria);
            copiarStringAVector(valorDeMemoria, registrosDeCpu.EBX, 8);
        }else if (strcmp(reg, "ECX") == 0){
            pedirleAMemoria(buffer, 8, valorDeMemoria);
            copiarStringAVector(valorDeMemoria, registrosDeCpu.ECX, 8);
        }else if (strcmp(reg, "EDX") == 0){
            pedirleAMemoria(buffer, 8, valorDeMemoria);
            copiarStringAVector(valorDeMemoria, registrosDeCpu.EDX, 8);
        }else if (strcmp(reg, "RAX") == 0){
            pedirleAMemoria(buffer, 16, valorDeMemoria);
            copiarStringAVector(valorDeMemoria, registrosDeCpu.RAX, 16);
        }else if (strcmp(reg, "RBX") == 0){
            pedirleAMemoria(buffer, 16, valorDeMemoria);
            copiarStringAVector(valorDeMemoria, registrosDeCpu.RBX, 16);
        }else if (strcmp(reg, "RCX") == 0){
            pedirleAMemoria(buffer, 16, valorDeMemoria);
            copiarStringAVector(valorDeMemoria, registrosDeCpu.RCX, 16);
        }else if (strcmp(reg, "RDX") == 0){
            pedirleAMemoria(buffer, 16, valorDeMemoria);
            copiarStringAVector(valorDeMemoria, registrosDeCpu.RDX, 16);
        } else {
            log_error(cpuLogger, "no se reconocio el registro para ejecutar MOV_IN");
            exit(-1);
        }
        free(valorDeMemoria);
    }else{
        empaquetar_contexto_para_kernel(buffer,programCounterActualizado,contexto);
        stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_terminado_seg_fault,buffer);
    }
    buffer_destruir(buffer);
}

void escribirEnMemoria(t_buffer* buffer, uint32_t cantidadDeBytes, char* reg){
    buffer_empaquetar(buffer, &cantidadDeBytes, sizeof(cantidadDeBytes));
    buffer_empaquetar(buffer, reg, cantidadDeBytes);
    stream_enviar_buffer(cpu_config_obtener_socket_memoria(cpuConfig), HEADER_valor_de_registro, buffer);
    int headerPuedeContinuar = stream_recibir_header(cpu_config_obtener_socket_memoria(cpuConfig));
    if(headerPuedeContinuar!= HEADER_OK_puede_continuar){
        log_error(cpuLogger, "error al escribir en MEMORIA");
        exit(-1);
    }
}


void ejecutar_MOV_OUT(t_contexto* contexto,uint32_t programCounterActualizado, char* reg, char* dirLogica){
    uint32_t pid = contexto_obtener_pid(contexto);
    log_info(cpuLogger, "PID: %d - Ejecutando: MOV_OUT ", pid);
    int direccionLogica = atoi(dirLogica);
    uint32_t nroSegmento = obtener_numero_de_segmento(direccionLogica);
    uint32_t offset = obtener_offset_de_segmento(direccionLogica);
    t_buffer* buffer = buffer_crear(); 
    if(offset < contexto_obtener_tabla_de_segmentos(contexto)[nroSegmento].tamanio){
        buffer_empaquetar(buffer,&pid, sizeof(pid));
        buffer_empaquetar(buffer,&nroSegmento, sizeof(nroSegmento));
        buffer_empaquetar(buffer,&offset, sizeof(offset));
        char* valorDeMemoria = malloc(sizeof(*valorDeMemoria));
        if (strcmp(reg, "AX") == 0){   
            escribirEnMemoria(buffer, 4, registrosDeCpu.AX);
        }else if (strcmp(reg, "BX") == 0){
            escribirEnMemoria(buffer, 4, registrosDeCpu.BX);
        }else if (strcmp(reg, "CX") == 0){
            escribirEnMemoria(buffer, 4, registrosDeCpu.CX);
        }else if (strcmp(reg, "DX") == 0){
            escribirEnMemoria(buffer, 4, registrosDeCpu.DX);
        }else if (strcmp(reg, "EAX") == 0){
            escribirEnMemoria(buffer, 8, registrosDeCpu.EAX);
        }else if (strcmp(reg, "EBX") == 0){
            escribirEnMemoria(buffer, 8, registrosDeCpu.EBX);
        }else if (strcmp(reg, "ECX") == 0){
            escribirEnMemoria(buffer, 8, registrosDeCpu.ECX);
        }else if (strcmp(reg, "EDX") == 0){
            escribirEnMemoria(buffer, 8, registrosDeCpu.EDX);
        }else if (strcmp(reg, "RAX") == 0){
            escribirEnMemoria(buffer, 16, registrosDeCpu.RAX);
        }else if (strcmp(reg, "RBX") == 0){
            escribirEnMemoria(buffer, 16, registrosDeCpu.RBX);
        }else if (strcmp(reg, "RCX") == 0){
            escribirEnMemoria(buffer, 16, registrosDeCpu.RCX);
        }else if (strcmp(reg, "RDX") == 0){
            escribirEnMemoria(buffer, 16, registrosDeCpu.RDX);
        } else {
            log_error(cpuLogger, "no se reconocio el registro para ejecutar MOV_OUT");
            exit(-1);
        }
        free(valorDeMemoria);
    }else{
        empaquetar_contexto_para_kernel(buffer,programCounterActualizado,contexto);
        stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_terminado_seg_fault,buffer);
    }
    buffer_destruir(buffer);
}

void ejecutar_CREATE_SEGMENT(t_contexto* contexto,uint32_t programCounterActualizado, char* IdsegmentoComoString, char* tamanioComoString){
    uint32_t id_segmento= atoi(IdsegmentoComoString);
    uint32_t tamanio_segmento = atoi(tamanioComoString);
    log_info(cpuLogger, "PID: %d - Ejecutando: CREATE_SEGMENT", contexto_obtener_pid(contexto));
    t_buffer *buffer = buffer_crear();
    empaquetar_contexto_para_kernel(buffer,programCounterActualizado,contexto);
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_create_segment, buffer);
    buffer_destruir(buffer);
    t_buffer* bufferParametros = buffer_crear();
    buffer_empaquetar(bufferParametros ,&id_segmento , sizeof(id_segmento));
    buffer_empaquetar(bufferParametros ,&tamanio_segmento , sizeof(tamanio_segmento));
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_parametros, bufferParametros);
    log_info(cpuLogger, "llego a enviar buffer ");
    buffer_destruir(bufferParametros); 
}  

void ejecutar_DELETE_SEGMENT(t_contexto* contexto,uint32_t programCounterActualizado, char* IdsegmentoComoString){
    uint32_t id_segmento = atoi(IdsegmentoComoString);
    log_info(cpuLogger, "PID: %d - Ejecutando: DELETE_SEGMENT", contexto_obtener_pid(contexto));
    t_buffer *buffer = buffer_crear();
    empaquetar_contexto_para_kernel(buffer,programCounterActualizado,contexto);
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_delete_segment, buffer);
    buffer_destruir(buffer);
    t_buffer* bufferParametros = buffer_crear();
    buffer_empaquetar(bufferParametros ,&id_segmento , sizeof(id_segmento));
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_parametros, bufferParametros);
    log_info(cpuLogger, "llego a enviar buffer ");
    buffer_destruir(bufferParametros); 
}

void ejecutar_WAIT(t_contexto* contexto,uint32_t programCounterActualizado, char* recurso){
    log_info(cpuLogger, "PID: %d - Ejecutando: WAIT", contexto_obtener_pid(contexto));
    t_buffer *bufferWAIT = buffer_crear();
    empaquetar_contexto_para_kernel(bufferWAIT,programCounterActualizado,contexto);
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_wait, bufferWAIT);
    buffer_destruir(bufferWAIT);

    t_buffer *bufferParametros = buffer_crear();
    buffer_empaquetar_string(bufferParametros, recurso);
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_parametros, bufferParametros);
    buffer_destruir(bufferParametros);
}

void ejecutar_SIGNAL(t_contexto* contexto,uint32_t programCounterActualizado, char* recurso){
    log_info(cpuLogger, "PID: %d - Ejecutando: SIGNAL", contexto_obtener_pid(contexto));
    t_buffer *buffer = buffer_crear();
    empaquetar_contexto_para_kernel(buffer,programCounterActualizado,contexto);
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_signal, buffer);
    buffer_destruir(buffer);

    t_buffer *bufferParametros = buffer_crear();
    buffer_empaquetar_string(bufferParametros, recurso);
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_parametros, bufferParametros);
    buffer_destruir(bufferParametros);
}

void ejecutar_IO(t_contexto* contexto, uint32_t programCounterActualizado, char* tiempoDeBloqueoComoString){
    uint32_t pid = contexto_obtener_pid(contexto);
    uint32_t tiempoDeBloqueo = atoi(tiempoDeBloqueoComoString);
    log_info(cpuLogger, "PID: %d - Ejecutando: I/O - de %d milisegundos", pid, tiempoDeBloqueo);
    t_buffer *bufferIO = buffer_crear();
    empaquetar_contexto_para_kernel(bufferIO,programCounterActualizado,contexto);;
    buffer_empaquetar(bufferIO, &tiempoDeBloqueo, sizeof(tiempoDeBloqueo));
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_bloqueado, bufferIO);
    buffer_destruir(bufferIO);
}


void ejecutar_EXIT(t_contexto* contexto,uint32_t programCounterActualizado){
    log_info(cpuLogger, "PID: %d - Ejecutando: EXIT", contexto_obtener_pid(contexto));
    t_buffer *bufferExit = buffer_crear();
    empaquetar_contexto_para_kernel(bufferExit,programCounterActualizado,contexto);
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_terminado, bufferExit);
    buffer_destruir(bufferExit);
}

void ejecutar_YIELD(t_contexto* contexto, uint32_t programCounterActualizado){
    log_info(cpuLogger, "PID: %d - Ejecutando: YIELD", contexto_obtener_pid(contexto));
    t_buffer* bufferSalida = buffer_crear();
    empaquetar_contexto_para_kernel(bufferSalida,programCounterActualizado,contexto);
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_yield, bufferSalida);
    buffer_destruir(bufferSalida);
}

void ejecutar_F_OPEN(t_contexto* contexto, uint32_t programCounterActualizado, char* NombreArchivo){
    log_info(cpuLogger, "PID: %d - Ejecutando: F_OPEN",contexto_obtener_pid(contexto));

    t_buffer* bufferF_OPEN = buffer_crear();
    empaquetar_contexto_para_kernel(bufferF_OPEN,programCounterActualizado,contexto);
    buffer_empaquetar_string(bufferF_OPEN, NombreArchivo);

    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_F_OPEN, bufferF_OPEN);
    buffer_destruir(bufferF_OPEN);   
}

void ejecutar_F_TRUNCATE(t_contexto* contexto, uint32_t programCounterActualizado, char* NombreArchivo, char* tamanioEnString){
    uint32_t tamanio = atoi(tamanioEnString);
    log_info(cpuLogger, "PID: %d - Ejecutando: F_TRUNCATE", contexto_obtener_pid(contexto));

    t_buffer* bufferF_TRUNCATE = buffer_crear();
     empaquetar_contexto_para_kernel(bufferF_TRUNCATE,programCounterActualizado,contexto);
    buffer_empaquetar_string(bufferF_TRUNCATE, NombreArchivo);
    buffer_empaquetar(bufferF_TRUNCATE, &tamanio, sizeof(tamanio));

    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_F_TRUNCATE, bufferF_TRUNCATE);
    buffer_destruir(bufferF_TRUNCATE);   
}

void ejecutar_F_SEEK(t_contexto* contexto,uint32_t programCounterActualizado, char* nombreArchivo, char* puntero){
    uint32_t punteroArchivo = atoi(puntero);
    log_info(cpuLogger, "PID: %d - Ejecutando: F_SEEK", contexto_obtener_pid(contexto));

    t_buffer* buffer_F_SEEK = buffer_crear();
    empaquetar_contexto_para_kernel(buffer_F_SEEK, programCounterActualizado, contexto);
    buffer_empaquetar_string(buffer_F_SEEK, nombreArchivo);
    buffer_empaquetar(buffer_F_SEEK, punteroArchivo, sizeof(punteroArchivo));

    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_F_SEEK, buffer_F_SEEK);
    buffer_destruir(buffer_F_SEEK);
}

 bool cpu_ejecutar_instrucciones(t_contexto* contexto, t_tipo_instruccion tipoInstruccion, char* parametro1, char* parametro2, char* parametro3) {
    contexto_setear_program_counter(contexto, contexto_obtener_program_counter(contexto) + 1);
    uint32_t programCounterActualizado = contexto_obtener_program_counter(contexto);
    bool pararDeEjecutar = false;

    switch (tipoInstruccion)
    {
    case INSTRUCCION_set:
        ejecutar_SET(contexto, parametro1, parametro2);
        pararDeEjecutar = false;
        break;
    case INSTRUCCION_f_close:
        pararDeEjecutar = true;
        break;
    case INSTRUCCION_f_open:
        ejecutar_F_OPEN(contexto, programCounterActualizado, parametro1);
        pararDeEjecutar = true;
        break;
    case INSTRUCCION_f_write:
        pararDeEjecutar = true;
        break;
    case INSTRUCCION_f_read:
        pararDeEjecutar = true;
        break;
    case INSTRUCCION_f_seek:
        ejecutar_F_SEEK(contexto, programCounterActualizado, parametro1, parametro2);
        pararDeEjecutar = true;
        break;
    case INSTRUCCION_f_truncate:
        ejecutar_F_TRUNCATE(contexto, programCounterActualizado, parametro1, parametro2);
        pararDeEjecutar = true;
        break;
    case INSTRUCCION_mov_in:
        ejecutar_MOV_IN(contexto,programCounterActualizado, parametro1, parametro2);
        pararDeEjecutar = false;
        break;
    case INSTRUCCION_mov_out:
        ejecutar_MOV_OUT(contexto,programCounterActualizado,parametro2,parametro1);
        pararDeEjecutar = false;
        break;
    case INSTRUCCION_create_segment:
        ejecutar_CREATE_SEGMENT(contexto, programCounterActualizado, parametro1, parametro2);
        pararDeEjecutar = true;
        break;
    case INSTRUCCION_delete_segment:
        ejecutar_DELETE_SEGMENT(contexto,programCounterActualizado, parametro1);
        pararDeEjecutar = true;
        break;
    case INSTRUCCION_wait:
        ejecutar_WAIT(contexto,programCounterActualizado, parametro1);
        pararDeEjecutar = true;
        break;
    case INSTRUCCION_signal:
        ejecutar_SIGNAL(contexto, programCounterActualizado, parametro1);
        pararDeEjecutar = true;
        break;
    case INSTRUCCION_yield:
        ejecutar_YIELD(contexto, programCounterActualizado);
        pararDeEjecutar = true;
        break;
    case INSTRUCCION_io:
        ejecutar_IO(contexto,programCounterActualizado,parametro1); // parametro1 es tiempoDeBloqueo
        pararDeEjecutar = true;
        break;
    case INSTRUCCION_exit:
        ejecutar_EXIT(contexto,programCounterActualizado);
        pararDeEjecutar = true;
        break;
    default:
        break;
    }

    return pararDeEjecutar;
}


bool cpu_ejecutar_ciclos_de_instruccion(t_contexto* contexto) {
    t_instruccion* siguienteInstruccion = cpu_fetch_instruccion(contexto);

    t_tipo_instruccion tipoInstruccion = instruccion_obtener_tipo_instruccion(siguienteInstruccion);
    char* parametro1 = instruccion_obtener_parametro1(siguienteInstruccion);
    char* parametro2 = instruccion_obtener_parametro2(siguienteInstruccion);
    char* parametro3 = instruccion_obtener_parametro3(siguienteInstruccion);

    return cpu_ejecutar_instrucciones(contexto, tipoInstruccion, parametro1, parametro2,parametro3);
}



void dispatch_peticiones_de_kernel(void) {
    uint32_t pidRecibido = 0;
    uint32_t programCounter = 0;

    for (;;) {
        uint8_t kernelRespuesta = stream_recibir_header(cpu_config_obtener_socket_kernel(cpuConfig));
        t_buffer* bufferContexto = NULL;
        t_contexto* contexto = NULL;
        log_info(cpuLogger,"kernelR %d procAEjec %d", kernelRespuesta, HEADER_proceso_a_ejecutar);
        if (kernelRespuesta != HEADER_proceso_a_ejecutar) {
            log_error(cpuLogger, "Error al intentar recibir el CONTEXTO de Kernel");
            exit(-1);
        }
            bufferContexto = buffer_crear();
            stream_recibir_buffer(cpu_config_obtener_socket_kernel(cpuConfig), bufferContexto);
            buffer_desempaquetar(bufferContexto, &pidRecibido, sizeof(pidRecibido));
            log_info(cpuLogger, "desempaquetado de pid %d", HEADER_proceso_a_ejecutar);
            buffer_desempaquetar(bufferContexto, &programCounter, sizeof(programCounter));
            log_info(cpuLogger, "desempaquetado de prgr");
            contexto = crear_contexto(pidRecibido, programCounter);
            buffer_desempaquetar_tabla_de_segmentos(bufferContexto, contexto_obtener_tabla_de_segmentos(contexto), cantidadDeSegmentos);
            log_info(cpuLogger, "desempaquetado de tab");
            buffer_desempaquetar_registros(bufferContexto, contexto_obtener_registros(contexto));
            log_info(cpuLogger, "desempaquetado de regs");
            buffer_destruir(bufferContexto);

            kernelRespuesta = stream_recibir_header(cpu_config_obtener_socket_kernel(cpuConfig));
            if (kernelRespuesta == HEADER_lista_de_instrucciones) {
                t_buffer* bufferInstrucciones = buffer_crear();
                stream_recibir_buffer(cpu_config_obtener_socket_kernel(cpuConfig), bufferInstrucciones);
                t_list* listaInstrucciones = instruccion_lista_crear_desde_buffer(bufferInstrucciones, cpuLogger);
                contexto_setear_instrucciones(contexto, listaInstrucciones);
                buffer_destruir(bufferInstrucciones);
            } else {
                log_error(cpuLogger, "Error al intentar recibir las instrucciones de Kernel");
                exit(-1);
            }
            bool pararDeEjecutar = false;
            while (!pararDeEjecutar) {
                pararDeEjecutar = cpu_ejecutar_ciclos_de_instruccion(contexto);
            }
            contexto_destruir(contexto);
    }
}

void atender_peticiones_de_kernel(void) {
    log_info(cpuLogger, "Listo para atender peticiones de Kernel");
    dispatch_peticiones_de_kernel();
}


