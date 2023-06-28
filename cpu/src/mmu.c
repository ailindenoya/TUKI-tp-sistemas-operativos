
#include "../include/mmu.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


extern t_cpu_config* cpuConfig;


uint32_t obtener_numero_de_segmento(int dirLogica){
    return floor(dirLogica / cpu_config_obtener_tam_max_segmento(cpuConfig));
}

uint32_t obtener_offset_de_segmento(int dirLogica){
    return dirLogica % cpu_config_obtener_tam_max_segmento(cpuConfig); 
}

