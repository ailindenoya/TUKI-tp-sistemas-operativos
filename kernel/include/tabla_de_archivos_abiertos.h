#ifndef TABLA_H_INCLUDED
#define TABLA_H_INCLUDED


#include <commons/collections/list.h>
#include <stdint.h>


struct t_archivo_tabla_proceso{
    char* nombreArchivo;
    uint32_t puntero;
};

typedef struct t_archivo_tabla_proceso t_archivo_tabla_proceso;

#endif