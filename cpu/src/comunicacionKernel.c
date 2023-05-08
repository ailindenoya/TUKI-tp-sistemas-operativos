#include "../include/comunicacionKernel.h"

extern t_log* cpuLogger;
extern t_cpu_config* cpuConfig;
//extern t_contexto contextoEjecucion = crear_contexto(...);   Obtener contexto enviado por Kernel y definirlo acá

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

void copiarStringAVector(char* string, char* vector, int tamanioDeRegistro) {
    for(int i = 0; i < tamanioDeRegistro; i++)
        vector[i] = string[i];
}

void ejecutar_SET(char* reg, char* param, t_contexto* pcb ) {
    

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
void ejecutar_MOV_IN(t_contexto* pcb,uint32_t programCounterActualizado){

}
void ejecutar_MOV_OUT(t_contexto* pcb,uint32_t programCounterActualizado){

}
void ejecutar_CREATE_SEGMENT(t_contexto* pcb,uint32_t programCounterActualizado){

}
void ejecutar_DELETE_SEGMENT(t_contexto* pcb,uint32_t programCounterActualizado){

}
void ejecutar_WAIT(t_contexto* pcb,uint32_t programCounterActualizado){

}
void ejecutar_SIGNAL(t_contexto* pcb,uint32_t programCounterActualizado){

}

void ejecutar_IO(char* tiempoDeBloqueo,t_contexto* pcb, uint32_t programCounterActualizado){
    uint32_t pid = contexto_obtener_pid(pcb);
    log_info(cpuLogger, "PCB de ID con I/O de %d milisegundos", pid, tiempoDeBloqueo);
    t_buffer *bufferIO = buffer_crear();
    buffer_empaquetar(bufferIO, &pid, sizeof(pid));
    buffer_empaquetar(bufferIO, &programCounterActualizado, sizeof(programCounterActualizado));
    buffer_empaquetar(bufferIO, &tiempoDeBloqueo, sizeof(tiempoDeBloqueo));
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_bloqueado, bufferIO);
    buffer_destruir(bufferIO);
}


void ejecutar_EXIT(t_contexto* pcb,uint32_t programCounterActualizado){
    log_info(cpuLogger, "PCB de ID %d ejecuta EXIT", contexto_obtener_pid(pcb));
    uint32_t pid = contexto_obtener_pid(pcb);
    t_buffer *bufferExit = buffer_crear();
    buffer_empaquetar(bufferExit, &pid, sizeof(pid));
    buffer_empaquetar(bufferExit, &programCounterActualizado, sizeof(programCounterActualizado));
    stream_enviar_buffer(cpu_config_obtener_socket_kernel(cpuConfig), HEADER_proceso_terminado, bufferExit);
    buffer_destruir(bufferExit);
}



 bool cpu_ejecutar_instrucciones(t_contexto* pcb, t_tipo_instruccion tipoInstruccion, char* parametro1, char* parametro2, char* parametro3) {
    contexto_setear_program_counter(pcb, contexto_obtener_program_counter(pcb) + 1);
    uint32_t programCounterActualizado = contexto_obtener_program_counter(pcb);
    bool pararDeEjecutar = false;

    switch (tipoInstruccion)
    {
    case INSTRUCCION_set:
        ejecutar_SET(parametro1, parametro2,pcb);
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
        break;
    case INSTRUCCION_mov_out:
        break;
    case INSTRUCCION_create_segment:
        break;
    case INSTRUCCION_delete_segment:
        break;
    case INSTRUCCION_wait:
        break;
    case INSTRUCCION_signal:
        break;
    case INSTRUCCION_yield:
        break;
    case INSTRUCCION_io:
        ejecutar_IO(parametro1, pcb, programCounterActualizado); // parametro1 es tiempoDeBloqueo
        pararDeEjecutar = true;
        return pararDeEjecutar;
        break;
    case INSTRUCCION_exit:
        ejecutar_EXIT(pcb,programCounterActualizado);
        pararDeEjecutar = true;
        return pararDeEjecutar;
        break;
    default:
        break;
    }
}