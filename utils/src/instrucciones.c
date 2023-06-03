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
    self->parametro1 = parametroUno;
    self->parametro2 = parametroDos;
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


t_list* instruccion_lista_crear_desde_buffer(t_buffer* bufferConInstrucciones, t_log* logger) {
    t_list* instrucciones = list_create();
    uint8_t instruccion = -1;
    bool esEXIT = false;
    while (!esEXIT) {
        buffer_desempaquetar(bufferConInstrucciones, &instruccion, sizeof(instruccion));
        char* parametro1 = malloc(1); // Lo pongo por las dudas, quizas despues sirva o quizas no
        strncpy(parametro1, "", 1);
        char* parametro2 = malloc(1);
        strncpy(parametro2, "", 1);
        char* parametro3 = malloc(1);
        strncpy(parametro3, "", 1);
        switch (instruccion) {
            case INSTRUCCION_set:
                instrucciones_desempaquetar_dos_parametros(bufferConInstrucciones, &parametro1, &parametro2);
                break;
            case INSTRUCCION_mov_in:
                instrucciones_desempaquetar_dos_parametros(bufferConInstrucciones, &parametro1, &parametro2);
                break;
            case INSTRUCCION_mov_out:
                instrucciones_desempaquetar_dos_parametros(bufferConInstrucciones, &parametro1, &parametro2);
                break;
            case INSTRUCCION_io:
                instrucciones_desempaquetar_un_parametro(bufferConInstrucciones, &parametro1);
                break;
            case INSTRUCCION_f_open:
                instrucciones_desempaquetar_un_parametro(bufferConInstrucciones, &parametro1);
                break;
            case INSTRUCCION_f_close:
                instrucciones_desempaquetar_un_parametro(bufferConInstrucciones, &parametro1);
                break;
            case INSTRUCCION_f_seek:
                instrucciones_desempaquetar_dos_parametros(bufferConInstrucciones, &parametro1, &parametro2);
                break;
            case INSTRUCCION_f_read:
                instrucciones_desempaquetar_tres_parametros(bufferConInstrucciones, &parametro1, &parametro2, &parametro3);
                break;
            case INSTRUCCION_f_write:
                instrucciones_desempaquetar_tres_parametros(bufferConInstrucciones, &parametro1, &parametro2, &parametro3);
                break;
            case INSTRUCCION_f_truncate:
                instrucciones_desempaquetar_dos_parametros(bufferConInstrucciones, &parametro1, &parametro2);
                break;
            case INSTRUCCION_wait:
                instrucciones_desempaquetar_un_parametro(bufferConInstrucciones, &parametro1);
                break;
            case INSTRUCCION_signal:
                instrucciones_desempaquetar_un_parametro(bufferConInstrucciones, &parametro1);
                break;
            case INSTRUCCION_create_segment:
                instrucciones_desempaquetar_dos_parametros(bufferConInstrucciones, &parametro1, &parametro2);
                break;
            case INSTRUCCION_delete_segment:
                instrucciones_desempaquetar_un_parametro(bufferConInstrucciones, &parametro1);
                break;
            case INSTRUCCION_yield:
                //No lleva ningun parametro
                break;
            case INSTRUCCION_exit:
                esEXIT = true;
                break;
            default:
                log_error(logger, "Error al intentar desempaquetar una instrucción");
                exit(-1);
        }
        t_instruccion* instruccionActual = instruccion_crear(instruccion, parametro1, parametro2, parametro3);
        list_add(instrucciones, instruccionActual);
    }
    log_info(logger, "Se desempaquetaron instrucciones");
    return instrucciones;
}

void instrucciones_desempaquetar_un_parametro(t_buffer* buffer, char** pParametro1) {
    buffer_desempaquetar_string(buffer, pParametro1);
}

void instrucciones_desempaquetar_dos_parametros(t_buffer* buffer, char** pParametro1, char** pParametro2) {
    instrucciones_desempaquetar_un_parametro(buffer, pParametro1);
    buffer_desempaquetar_string(buffer, pParametro2);
}

void instrucciones_desempaquetar_tres_parametros(t_buffer* buffer, char** pParametro1, char** pParametro2, char** pParametro3) {
    instrucciones_desempaquetar_dos_parametros(buffer, pParametro1, pParametro2);
    buffer_desempaquetar_string(buffer, pParametro3);
}
