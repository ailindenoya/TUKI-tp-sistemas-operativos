#include "../include/comunicacionKernel.h"

extern t_log* cpuLogger;
extern t_cpu_config* cpuConfig;
//extern t_contexto contextoEjecucion = crear_contexto(...);   Obtener contexto enviado por Kernel y definirlo acá


static int pidProcesoEnExec;
 // me parece bien dejarlas como static por las dudas

char AX[4];
char BX[4];
char CX[4];
char DX[4];
char EAX[8];
char EBX[8];
char ECX[8];
char EDX[8];
char RAX[16];
char RBX[16];
char RCX[16];
char RDX[16];

t_instruccion* cpu_fetch_instruccion(t_contexto* pcb) {
    t_list* listaInstrucciones = contexto_obtener_instrucciones(pcb);
    uint32_t programCounter = contexto_obtener_program_counter(pcb);
    t_instruccion* instruccionSig = list_get(listaInstrucciones, programCounter);
    log_info(cpuLogger, "FETCH INSTRUCCION: PCB <ID %d>", contexto_obtener_pid(pcb));
    return instruccionSig;
}

/*uint32_t cpu_fetch_parametros(t_instruccion* siguienteInstruccion, t_contexto* pcb) {
    char* direccionLogicaOrigen = instruccion_obtener_parametro1(siguienteInstruccion);
    printf("CPU tabla página primer nivel en CPU_fetch_operands: %d\n", cpu_pcb_get_tabla_pagina_primer_nivel(pcb));  
    uint32_t fetchedValue = cpu_leer_en_memoria(tlb, cpu_config_get_socket_memoria(cpuConfig), direccionLogicaOrigen, cpu_pcb_get_tabla_pagina_primer_nivel(pcb));
    log_info(cpuLogger, "FETCH OPERANDS: PCB: %d COPY <DL Destino: %d> <DL Origen: %d> => Fetched Value: %d", contexto_obtener_pid(pcb), instruccion_obtener_parametro1(siguienteInstruccion), direccionLogicaOrigen, fetchedValue);
    return fetchedValue;
}*/   
// Hay que ver mejor esto cuando veamos MEMORIA, y tengamos que hacer instrucciones con direcciones lógicas como parámetros

void copiarStringAVector(char* string, char* vector, int tamanioDeRegistro) {
    for(int i = 0; i < tamanioDeRegistro; i++)
        vector[i] = string[i];
}

void ejecutar_SET(t_contexto* pcb, char* reg, char* param) {
    
    log_info(cpuLogger, "PID: %d - Ejecutando: SET ", contexto_obtener_pid(pcb));
    uint32_t retardo = (cpu_config_obtener_retardo_instruccion(cpuConfig))/1000;
    sleep(retardo);

    if(strcmp(reg,"AX") == 0){
        copiarStringAVector(param, AX, 4);
    }else if (strcmp(reg,"BX") == 0){
        copiarStringAVector(param, BX, 4);
    }else if (strcmp(reg,"CX") == 0){
        copiarStringAVector(param, CX, 4);
    }else if (strcmp(reg,"DX") == 0){
        copiarStringAVector(param, DX, 4);
    }else if (strcmp(reg,"EAX") == 0){
        copiarStringAVector(param, EAX, 8);
    }else if (strcmp(reg,"EBX") == 0){
        copiarStringAVector(param, EBX, 8);
    }else if (strcmp(reg,"ECX") == 0){
        copiarStringAVector(param, ECX, 8);
    }else if (strcmp(reg,"EDX") == 0){
        copiarStringAVector(param, EDX, 8);
    }else if (strcmp(reg,"RAX") == 0){
        copiarStringAVector(param, RAX, 16);
    }else if (strcmp(reg,"RBX") == 0){
        copiarStringAVector(param, RBX, 16);
    }else if (strcmp(reg,"RCX") == 0){
        copiarStringAVector(param, RCX, 16);
    }else if (strcmp(reg,"RDX") == 0){
        copiarStringAVector(param, RDX, 16);
    }else {
        log_info(cpuLogger, "error al ejecutar SET");
    }
}

void ejecutar_F_CLOSE(t_contexto* pcb,uint32_t programCounterActualizado){

}
void ejecutar_F_OPEN(t_contexto* pcb,uint32_t programCounterActualizado){

}
void ejecutar_F_READ(t_contexto* pcb,uint32_t programCounterActualizado){

}
void ejecutar_F_WRITE(t_contexto* pcb,uint32_t programCounterActualizado){

}
void ejecutar_F_SEEK(t_contexto* pcb,uint32_t programCounterActualizado){

}
void ejecutar_F_TRUNCATE(t_contexto* pcb,uint32_t programCounterActualizado){

}
void ejecutar_MOV_IN(t_contexto* pcb,uint32_t programCounterActualizado, char* reg, char* dirLogica){
    log_info(cpuLogger, "PID: %d - Ejecutando: MOV_IN ", contexto_obtener_pid(pcb));
    // se sobreescribe lo que ocurre en set???? o hay que crear nuevos?
    if(strcmp(reg,"AX") == 0){
        copiarStringAVector(dirLogica, AX, 4);
    }else if (strcmp(reg,"BX") == 0){
        copiarStringAVector(dirLogica, BX, 4);
    }else if (strcmp(reg,"CX") == 0){
        copiarStringAVector(dirLogica, CX, 4);
    }else if (strcmp(reg,"DX") == 0){
        copiarStringAVector(dirLogica, DX, 4);
    }else if (strcmp(reg,"EAX") == 0){
        copiarStringAVector(dirLogica, EAX, 8);
    }else if (strcmp(reg,"EBX") == 0){
        copiarStringAVector(dirLogica, EBX, 8);
    }else if (strcmp(reg,"ECX") == 0){
        copiarStringAVector(dirLogica, ECX, 8);
    }else if (strcmp(reg,"EDX") == 0){
        copiarStringAVector(dirLogica, EDX, 8);
    }else if (strcmp(reg,"RAX") == 0){
        copiarStringAVector(dirLogica, RAX, 16);
    }else if (strcmp(reg,"RBX") == 0){
        copiarStringAVector(dirLogica, RBX, 16);
    }else if (strcmp(reg,"RCX") == 0){
        copiarStringAVector(dirLogica, RCX, 16);
    }else if (strcmp(reg,"RDX") == 0){
        copiarStringAVector(dirLogica, RDX, 16);
    }else {
        log_info(cpuLogger, "error al ejecutar MOV_IN");
    }
}

void ejecutar_MOV_OUT(t_contexto* pcb,uint32_t programCounterActualizado, char* dirLogica, char* regALeer){
    
}

void ejecutar_CREATE_SEGMENT(t_contexto* pcb,uint32_t programCounterActualizado, char* IdsegmentoComoString, char* tamanioComoString){
    uint32_t pid = contexto_obtener_pid(pcb);
    uint32_t id_segmento= atoi(IdsegmentoComoString);
    uint32_t tamanio_segmento = atoi(tamanioComoString);
    log_info(cpuLogger, "PID: %d - Ejecutando: CREATE_SEGMENT", contexto_obtener_pid(pcb));
    t_buffer *buffer = buffer_crear();
    buffer_empaquetar(buffer, &pid, sizeof(pid));
    buffer_empaquetar(buffer, &programCounterActualizado, sizeof(programCounterActualizado));
    buffer_empaquetar(buffer ,&id_segmento , sizeof(id_segmento));
    buffer_empaquetar(buffer ,&tamanio_segmento , sizeof(tamanio_segmento));
    stream_enviar_buffer(cpu_config_obtener_socket_memoria(cpuConfig), HEADER_create_segment, buffer);
    buffer_destruir(buffer);
}  

void ejecutar_DELETE_SEGMENT(t_contexto* pcb,uint32_t programCounterActualizado, char* IdsegmentoComoString){
    uint32_t pid = contexto_obtener_pid(pcb);
    uint32_t id_segmento = atoi(IdsegmentoComoString);
    log_info(cpuLogger, "PID: %d - Ejecutando: DELETE_SEGMENT", contexto_obtener_pid(pcb));
    t_buffer *buffer = buffer_crear();
    buffer_empaquetar(buffer, &pid, sizeof(pid));
    buffer_empaquetar(buffer, &programCounterActualizado, sizeof(programCounterActualizado));
    buffer_empaquetar(buffer ,&id_segmento , sizeof(id_segmento));
    stream_enviar_buffer(cpu_config_obtener_socket_memoria(cpuConfig), HEADER_delete_segment, buffer);
    buffer_destruir(buffer);
}

void ejecutar_WAIT(t_contexto* pcb,uint32_t programCounterActualizado, char* recurso){
    uint32_t pid = contexto_obtener_pid(pcb);
    log_info(cpuLogger, "PID: %d - Ejecutando: WAIT", contexto_obtener_pid(pcb));
    t_buffer *bufferWAIT = buffer_crear();
    buffer_empaquetar(bufferWAIT, &pid, sizeof(pid));
    buffer_empaquetar(bufferWAIT, &programCounterActualizado, sizeof(programCounterActualizado));
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_wait, bufferWAIT);
    buffer_destruir(bufferWAIT);

    t_buffer *bufferParametros = buffer_crear();
    buffer_empaquetar_string(bufferParametros, recurso);
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_parametros, bufferParametros);
    buffer_destruir(bufferParametros);
}

void ejecutar_SIGNAL(t_contexto* pcb,uint32_t programCounterActualizado, char* recurso){
    uint32_t pid = contexto_obtener_pid(pcb);
    log_info(cpuLogger, "PID: %d - Ejecutando: SIGNAL", contexto_obtener_pid(pcb));
    t_buffer *buffer = buffer_crear();
    buffer_empaquetar(buffer, &pid, sizeof(pid));
    buffer_empaquetar(buffer, &programCounterActualizado, sizeof(programCounterActualizado));
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_signal, buffer);
    buffer_destruir(buffer);

    t_buffer *bufferParametros = buffer_crear();
    buffer_empaquetar_string(bufferParametros, recurso);
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_parametros, bufferParametros);
    buffer_destruir(bufferParametros);
}

void ejecutar_IO(t_contexto* pcb, uint32_t programCounterActualizado, char* tiempoDeBloqueoComoString){
    uint32_t pid = contexto_obtener_pid(pcb);
    uint32_t tiempoDeBloqueo = atoi(tiempoDeBloqueoComoString);
    log_info(cpuLogger, "PID: %d - Ejecutando: I/O - de %d milisegundos", pid, tiempoDeBloqueo);
    t_buffer *bufferIO = buffer_crear();
    buffer_empaquetar(bufferIO, &pid, sizeof(pid));
    buffer_empaquetar(bufferIO, &programCounterActualizado, sizeof(programCounterActualizado));
    buffer_empaquetar(bufferIO, &tiempoDeBloqueo, sizeof(tiempoDeBloqueo));
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_bloqueado, bufferIO);
    buffer_destruir(bufferIO);
}


void ejecutar_EXIT(t_contexto* pcb,uint32_t programCounterActualizado){
    log_info(cpuLogger, "PID: %d - Ejecutando: EXIT", contexto_obtener_pid(pcb));
    uint32_t pid = contexto_obtener_pid(pcb);
    t_buffer *bufferExit = buffer_crear();
    buffer_empaquetar(bufferExit, &pid, sizeof(pid));
    buffer_empaquetar(bufferExit, &programCounterActualizado, sizeof(programCounterActualizado));
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_terminado, bufferExit);
    buffer_destruir(bufferExit);
}

void ejecutar_YIELD(t_contexto* pcb, uint32_t programCounterActualizado){
    uint32_t pid = contexto_obtener_pid(pcb);
    log_info(cpuLogger, "PID: %d - Ejecutando: YIELD", pid);
    t_buffer* bufferSalida = buffer_crear();
    buffer_empaquetar(bufferSalida, &pid, sizeof(pid));
    buffer_empaquetar(bufferSalida, &programCounterActualizado, sizeof(programCounterActualizado));
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_yield, bufferSalida);
    buffer_destruir(bufferSalida);
}



 bool cpu_ejecutar_instrucciones(t_contexto* pcb, t_tipo_instruccion tipoInstruccion, char* parametro1, char* parametro2, char* parametro3) {
    contexto_setear_program_counter(pcb, contexto_obtener_program_counter(pcb) + 1);
    uint32_t programCounterActualizado = contexto_obtener_program_counter(pcb);
    bool pararDeEjecutar = false;

    switch (tipoInstruccion)
    {
    case INSTRUCCION_set:
        ejecutar_SET(pcb, parametro1, parametro2);
        break;
    case INSTRUCCION_f_close:
        break;
    case INSTRUCCION_f_open:
        break;
    case INSTRUCCION_f_write:
        break;
    case INSTRUCCION_f_read:
        break;
    case INSTRUCCION_f_seek:
        break;
    case INSTRUCCION_f_truncate:
        break;
    case INSTRUCCION_mov_in:
        ejecutar_MOV_IN(pcb,programCounterActualizado,parametro1, parametro2);
        break;
    case INSTRUCCION_mov_out:
        ejecutar_MOV_OUT(pcb,programCounterActualizado,parametro1, parametro2);
        break;
    case INSTRUCCION_create_segment:
        ejecutar_CREATE_SEGMENT(pcb,programCounterActualizado,parametro1, parametro2);
        break;
    case INSTRUCCION_delete_segment:
        ejecutar_DELETE_SEGMENT(pcb, programCounterActualizado, parametro1);
        break;
    case INSTRUCCION_wait:
        ejecutar_WAIT(pcb,programCounterActualizado, parametro1);
        pararDeEjecutar = true;
        break;
    case INSTRUCCION_signal:
        ejecutar_SIGNAL(pcb, programCounterActualizado, parametro1);
        pararDeEjecutar = true;
        break;
    case INSTRUCCION_yield:
        ejecutar_YIELD(pcb, programCounterActualizado);
        pararDeEjecutar = true;
        break;
    case INSTRUCCION_io:
        ejecutar_IO(pcb,programCounterActualizado,parametro1); // parametro1 es tiempoDeBloqueo
        pararDeEjecutar = true;
        break;
    case INSTRUCCION_exit:
        ejecutar_EXIT(pcb,programCounterActualizado);
        pararDeEjecutar = true;
        break;
    default:
        break;
    }

    return pararDeEjecutar;
}


bool cpu_ejecutar_ciclos_de_instruccion(t_contexto* pcb) {
    t_instruccion* siguienteInstruccion = cpu_fetch_instruccion(pcb);

    ///hacerFetchDeParametros =  instruccion_obtener_tipo_instruccion(siguienteInstruccion);

    t_tipo_instruccion tipoInstruccion = instruccion_obtener_tipo_instruccion(siguienteInstruccion);
    char* parametro1 = instruccion_obtener_parametro1(siguienteInstruccion);
    char* parametro2 = instruccion_obtener_parametro2(siguienteInstruccion);
    char* parametro3 = instruccion_obtener_parametro3(siguienteInstruccion);

/*
    if (hacerFetchDeParametros) {
        A COMPLETAR CUANDO VEAMOS MEMORIA 
        hacer la interpretacion de memoria logica a fisica decodeando los parametros
        parametro2 = cpu_fetch_parametros(siguienteInstruccion, pcb);
    }*/

    return cpu_ejecutar_instrucciones(pcb, tipoInstruccion, parametro1, parametro2,parametro3);
}



void dispatch_peticiones_de_kernel(void) {
    uint32_t pidRecibido = 0;
    uint32_t programCounter = 0;
    for (;;) {
        uint8_t kernelRespuesta = stream_recibir_header(cpu_config_obtener_socket_kernel(cpuConfig));
        t_buffer* bufferPcb = NULL;
        t_contexto* pcb = NULL;
        if (kernelRespuesta == HEADER_pcb_a_ejecutar) {
            bufferPcb = buffer_crear();
            stream_recibir_buffer(cpu_config_obtener_socket_kernel(cpuConfig), bufferPcb);
            buffer_desempaquetar(bufferPcb, &pidRecibido, sizeof(pidRecibido));
            buffer_desempaquetar(bufferPcb, &programCounter, sizeof(programCounter));
            buffer_destruir(bufferPcb);
            if (pidRecibido != pidProcesoEnExec) {
                // flush de entradas (?) memoria
                pidProcesoEnExec = pidRecibido;
            }
            pcb = crear_contexto(pidRecibido, programCounter);
            kernelRespuesta = stream_recibir_header(cpu_config_obtener_socket_kernel(cpuConfig));
            if (kernelRespuesta == HEADER_lista_de_instrucciones) {
                t_buffer* bufferInstrucciones = buffer_crear();
                stream_recibir_buffer(cpu_config_obtener_socket_kernel(cpuConfig), bufferInstrucciones);
                t_list* listaInstrucciones = instruccion_lista_crear_desde_buffer(bufferInstrucciones, cpuLogger);
                contexto_setear_instrucciones(pcb, listaInstrucciones);
                buffer_destruir(bufferInstrucciones);
            } else {
                log_error(cpuLogger, "Error al intentar recibir las instrucciones de Kernel");
                exit(-1);
            }
            bool pararDeEjecutar = false;
            while (!pararDeEjecutar) {
                pararDeEjecutar = cpu_ejecutar_ciclos_de_instruccion(pcb);
           /*   no tenemos socket interrupt:
             if (!pararDeEjecutar) {
                    pararDeEjecutar = cpu_atender_interrupcion(pcb);
                }*/ 
            }
            contexto_destruir(pcb);
        } else {
            log_error(cpuLogger, "Error al intentar recibir el PCB de Kernel");
            exit(-1);
        }
    }
}

void atender_peticiones_de_kernel(void) {
    pidProcesoEnExec = -1;
    log_info(cpuLogger, "Listo para atender peticiones de Kernel");
    dispatch_peticiones_de_kernel();
}


