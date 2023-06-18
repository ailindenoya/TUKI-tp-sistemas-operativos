#ifndef MMU_H_INCLUDED
#define MMU_H_INCLUDED

#include "cpu_config.h"
#include <string.h>

int convertir_dir_fisica_a_logica(int dirLogica);
char obtener_numero_de_segmento(int dirLogica);
char obtener_offset_de_segmento(int dirLogica);


#endif 