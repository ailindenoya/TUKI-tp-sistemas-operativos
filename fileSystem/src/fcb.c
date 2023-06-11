#include "../include/fcb.h"

extern t_log* fileSystemLogger;

struct t_fcb {
    char* NOMBRE_ARCHIVO;
    uint32_t TAMANIO_ARCHIVO;
    uint32_t PUNTERO_DIRECTO;   // Primer bloque donde están los datos del archivo
    uint32_t PUNTERO_INDIRECTO; // Puntero a los siguientes demás bloques donde hay datos del archivo
};

t_fcb* fcb_crear(char* Nombre){
    t_fcb* self = malloc(sizeof(self));

    self->NOMBRE_ARCHIVO = Nombre;
    self->TAMANIO_ARCHIVO = 0;
    self->PUNTERO_DIRECTO = 0;
    self->PUNTERO_INDIRECTO = 0;

    return self;
}

char* fcb_obtener_nombre_archivo(t_fcb* self){
    return self->NOMBRE_ARCHIVO;
}

uint32_t fcb_obtener_tamanio_archivo(t_fcb* self){
    return self->TAMANIO_ARCHIVO;
}

uint32_t fcb_obtener_puntero_directo(t_fcb* self){
    return self->PUNTERO_DIRECTO;
}

uint32_t fcb_obtener_puntero_indirecto(t_fcb* self){
    return self->PUNTERO_INDIRECTO;
}