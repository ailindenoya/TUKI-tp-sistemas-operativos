#include "../include/pcb.h"

struct t_pcb {
    uint32_t pid;
    t_buffer* instrucciones;
    uint32_t programCounter;
    time_t tiempoDellegadaAReady;
    // registros de uso gral de cpu (??? )
    // tabla de segmentos
    double estimacionProximaRafaga;
    // tabla de archivos abiertos con LA info de la POSICION del puntero en cada uno (struct con puntero indicando posicion)

};