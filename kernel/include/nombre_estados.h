#ifndef NOMBRE_ESTADOS_H_INCLUDED
#define NOMBRE_ESTADOS_H_INCLUDED

typedef enum {
    NEW,
    READY,
    EXEC,
    EXIT,
    BLOCKED
    //PCBS_ESPERANDO_PARA_IO
} t_nombre_estado;

#endif