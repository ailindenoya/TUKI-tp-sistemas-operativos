#ifndef COMUNICACIONMEMORIA_H_INCLUDED
#define COMUNICACIONMEMORIA_H_INCLUDED


#include "../include/kernel_config.h"
#include "../include/pcb.h"
#include <pthread.h>
#include "../include/estado.h"
#include "kernel_config.h"

void memoria_finalizar_proceso(t_pcb* pcbAFinalizar, t_kernel_config* kernelConfig, t_log* kernelLogger);




#endif