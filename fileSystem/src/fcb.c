#include "../include/fcb.h"

extern t_log* fileSystemLogger;

struct t_fcb {
    char* NOMBRE_ARCHIVO;
    int TAMANIO_ARCHIVO;
    uint32_t PUNTERO_DIRECTO;
    uint32_t PUNTERO_INDIRECTO;
};

