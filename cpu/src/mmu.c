
#include "../include/mmu.h"


extern t_cpu_config* cpuConfig;

int convertir_dir_fisica_a_logica(int dirLogica){
    return atoi(strcat(obtener_numero_de_segmento(dirLogica), obtener_offset_de_segmento(dirLogica)));
}

char obtener_numero_de_segmento(int dirLogica){
    return toString(floor(dirLogica / atoi(cpu_config_obtener_tam_max_segmento(cpuConfig))));
}

char obtener_offset_de_segmento(int dirLogica){
    return toString(dirLogica % atoi(cpu_config_obtener_tam_max_segmento(cpuConfig)));
}

