#ifndef PLANIFICADOR_H_INCLUDED
#define PLANIFICADOR_H_INCLUDED

#include "kernel_config.h"
#include "pcb.h"
#include "estado.h"
#include <pthread.h>
#include "comunicacionCPU.h"

void loggear_cambio_estado(const char *prev, const char *post, int pid);
uint32_t obtener_siguiente_pid(void);
void* encolar_en_new_nuevo_pcb_entrante(void*);
#endif