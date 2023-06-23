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
    HEADER_proceso_signal,


    HEADER_proceso_F_OPEN,
    HEADER_proceso_F_CLOSE,
    HEADER_proceso_F_READ,
    HEADER_proceso_F_WRITE,
    HEADER_proceso_F_SEEK,
    HEADER_proceso_F_TRUNCATE,

    HEADER_cantidad_seg_enviada,
    HEADER_proceso_agregado_a_memoria,
    HEADER_proceso_a_agregar_a_memoria,
    HEADER_create_segment,
    HEADER_segmento_creado,
    
    HEADER_delete_segment,
    HEADER_mov_in,
    HEADER_mov_out,
    HEADER_existe_archivo,
    HEADER_proceso_terminado_out_of_memory,
    HEADER_hay_que_compactar,

    HEADER_archivo_abierto,
    HEADER_no_existe_archivo,
    HEADER_crear_archivo,
    HEADER_F_OPEN
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