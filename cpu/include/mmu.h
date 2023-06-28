#ifndef MMU_H_INCLUDED
#define MMU_H_INCLUDED

#include "cpu_config.h"
#include <string.h>

uint32_t obtener_numero_de_segmento(int dirLogica);
uint32_t obtener_offset_de_segmento(int dirLogica);


#endif 