#ifndef MMU_H_INCLUDED
#define MMU_H_INCLUDED

#include "cpu_config.h"
#include <string.h>

int convertir_dir_fisica_a_logica(int dirLogica);
int obtener_numero_de_segmento(int dirLogica);
int obtener_offset_de_segmento(int dirLogica);


#endif 