#ifndef COMMON_FLAGS_H_INCLUDED
#define COMMON_FLAGS_H_INCLUDED


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
    INSTRUCCION_f_read,
    INSTRUCCION_f_write,
    INSTRUCCION_set,
    INSTRUCCION_mov_in,
    INSTRUCCION_mov_out,
    INSTRUCCION_f_truncate,
    INSTRUCCION_f_seek,
    INSTRUCCION_f_create_segment,
    INSTRUCCION_io,
    INSTRUCCION_wait,
    INSTRUCCION_signal,
    INSTRUCCION_f_open,
    INSTRUCCION_f_close,
    INSTRUCCION_delete_segment,
    INSTRUCCION_exit,
    INSTRUCCION_yield
} t_tipo_instruccion;


#endif