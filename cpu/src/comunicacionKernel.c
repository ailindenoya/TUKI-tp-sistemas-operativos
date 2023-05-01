#include "../include/comunicacionKernel.h"

extern t_log* cpuLogger;
//extern t_contexto contextoEjecucion = crear_contexto(...);   Obtener contexto enviado por Kernel y definirlo acá
extern t_contexto t_contexto;



void copiarStringAVector(char* string, char* vector, int tamanioDeRegistro) {
    for(int i = 0; i < tamanioDeRegistro; i++)
        vector[i] = (string+i);
}

void ejecutar_SET(char* reg, char* param, t_contexto contexto) {

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
        copiarStringAVector(param, EAX, 16);
    }else if (strcmp(reg,"RBX") == 0){
        copiarStringAVector(param, EBX, 16);
    }else if (strcmp(reg,"RCX") == 0){
        copiarStringAVector(param, ECX, 16);
    }else if (strcmp(reg,"RDX") == 0){
        copiarStringAVector(param, EDX, 16);
    }else {
        log_info(cpuLogger, "error al ejecutar SET");
    }
}

void ejecutar_YIELD(){
    // Devolver contexto a Kernel
    
    // Enviar proceso a lista READY en Planificador

    t_list listaReady = estado_obtener_lista(READY);
    // Encolar Proceso
}

void ejecutar_EXIT(){
    // Devolver contexto a Kernel
    // Finalizar Proceso, enviar a lista EXIT en Planificador

    t_list listaExit = estado_obtener_lista(EXIT);
    // Agregar proceso a Lista
}

 bool __cpu_exec_instruction(t_contexto* pcb, t_tipo_instruccion tipoInstruccion, char* parametro1, char* parametro2, char* parametro3  ) {

    switch (tipoInstruccion)
    {
    case INSTRUCCION_set:
        ejecutar_SET(parametro1, parametro2);
        break;

    case INSTRUCCION_yield:

        break;
    case INSTRUCCION_exit:
        log_info(cpuLogger, "EXEC: PCB <ID %d> EXIT", _obtener_pid(pcb));

        break;
    default:
        break;
    }
}