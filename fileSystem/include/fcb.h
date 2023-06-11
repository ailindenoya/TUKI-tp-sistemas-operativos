#include <stdlib.h>
#include "fileSystem_config.h"

typedef struct t_fcb t_fcb;

t_fcb* fcb_crear(char*);

char* fcb_obtener_nombre_archivo(t_fcb*);
uint32_t fcb_obtener_tamanio_archivo(t_fcb*);
uint32_t fcb_obtener_puntero_directo(t_fcb*);
uint32_t fcb_obtener_puntero_indirecto(t_fcb*);