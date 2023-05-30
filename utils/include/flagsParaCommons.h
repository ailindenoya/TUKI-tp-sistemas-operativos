#ifndef FLAGSPARACOMMONS_H_INCLUDED
#define FLAGSPARACOMMONS_H_INCLUDED

typedef enum {
    HEADER_lista_de_instrucciones,
    HEADER_pid,
    HEADER_pcb_a_ejecutar,
    HEADER_proceso_bloqueado,
    HEADER_proceso_terminado,
    HEADER_proceso_yield,
    HEADER_proceso_wait,
    HEADER_proceso_parametros,
    HEADER_proceso_signal
} t_header;

typedef enum {
    HANDSHAKE_consola,
    HANDSHAKE_memoria,
    HANDSHAKE_cpu,
    HANDSHAKE_filesystem,
    HANDSHAKE_kernel,
    HANDSHAKE_interrupcion,
    HANDSHAKE_puede_continuar
} t_handshake;


typedef enum {
    INSTRUCCION_set,
    INSTRUCCION_mov_in,
    INSTRUCCION_mov_out,
    INSTRUCCION_io,
    INSTRUCCION_f_open,
    INSTRUCCION_f_close,
    INSTRUCCION_f_seek,
    INSTRUCCION_f_read,
    INSTRUCCION_f_write,
    INSTRUCCION_f_truncate,
    INSTRUCCION_wait,
    INSTRUCCION_signal,
    INSTRUCCION_create_segment,
    INSTRUCCION_delete_segment,
    INSTRUCCION_yield,
    INSTRUCCION_exit
} t_tipo_instruccion;


#endif