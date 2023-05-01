#include <kernel_config.h>
#include <pcb.h>
#include <estado.h>
#include <pthread.h>
#include "../../kernel/include/comunicacionCPU.h"

void loggear_cambio_estado(const char* prev, const char* post, int pid);
uint32_t obtener_siguiente_pid(void);
