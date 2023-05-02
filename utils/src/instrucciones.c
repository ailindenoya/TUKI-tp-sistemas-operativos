#include "../include/instrucciones.h"

struct t_instruccion {
    t_tipo_instruccion tipoInstruccion;
    char* parametro1;
    char* parametro2;
    char* parametro3;
};

t_instruccion* instruccion_crear(t_tipo_instruccion tipoInstruccion, char* parametroUno, char* parametroDos, char* parametroTres) {
    t_instruccion* self = malloc(sizeof(*self));
    self->tipoInstruccion = tipoInstruccion;
    self->operando1 = parametroUno;
    self->operando2 = parametroDos;
    self->parametro3 = parametroTres;
    return self;
}

void instruccion_destruir(void* selfVoid) {
    t_instruccion* self = (t_instruccion*)selfVoid;
    free(self->parametro1);
    free(self->parametro2);
    free(self->parametro3);
    free(self);
}

t_tipo_instruccion instruccion_obtener_tipo_instruccion(t_instruccion* self) {
    return self->tipoInstruccion;
}

char* instruccion_obtener_parametro1(t_instruccion* self) {
    return self->parametro1;
}

char* instruccion_obtener_parametro2(t_instruccion* self) {
    return self->parametro2;
}

char* instruccion_obtener_parametro3(t_instruccion* self) {
    return self->parametro3;
}


// CAMBIAR ESTO 
t_list* instruccion_lista_crear_desde_buffer(t_buffer* bufferConInstrucciones, t_log* logger) {
    t_list* instrucciones = list_create();
    uint8_t instruccion = -1;
    bool esEXIT = false;
    while (!isExit) {
        buffer_unpack(bufferConInstrucciones, &instruccion, sizeof(instruccion));
        uint32_t op1 = -1;
        uint32_t op2 = -1;
        switch (instruccion)
        {
        case INSTRUCCION_set:
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
        case INSTRUCCION_exit:
            esEXIT = true;
            break;
        default:
            log_error(logger, "Error al intentar desempaquetar una instrucción");
            exit(-1);
        }
        t_instruccion* instruccionActual = instruccion_create(instruction, op1, op2);
        list_add(instrucciones, instruccionActual);
    }
    log_info(logger, "Se desempaquetan %d instrucciones", list_size(instrucciones));
    return instrucciones;
}