
#include "../include/mmu.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


extern t_cpu_config* cpuConfig;


int concatenar(int x, int  y) {
    int pow = 10;
    while(y >= pow)
        pow *= 10;
    return x * pow + y;        
}

int convertir_dir_fisica_a_logica(int dirLogica){
    return concatenar(obtener_numero_de_segmento(dirLogica), obtener_offset_de_segmento(dirLogica));
}

int obtener_numero_de_segmento(int dirLogica){
    return floor(dirLogica / atoi(cpu_config_obtener_tam_max_segmento(cpuConfig)))// convertir a char;
}

int obtener_offset_de_segmento(int dirLogica){
    return dirLogica % atoi(cpu_config_obtener_tam_max_segmento(cpuConfig)) // convertir a char;
}

