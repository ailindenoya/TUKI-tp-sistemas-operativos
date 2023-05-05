#ifndef COMUNICACIONCPU_H_INCLUDED
#define COMUNICACIONCPU_H_INCLUDED

#include <stdlib.h>
#include <stdint.h>

#include "../include/kernel_config.h"
#include "../include/pcb.h"
#include "../../utils/include/flagsParaCommons.h"
#include "../../utils/include/stream.h"
#include <commons/log.h>

void kernel_enviar_pcb_a_cpu(t_pcb* pcbAEnviar, t_kernel_config* kernelConfig, t_log* kernelLogger, uint8_t header);
t_pcb* kernel_recibir_pcb_actualizado_de_cpu(t_pcb* pcbAActualizar, uint8_t cpuResponse, t_kernel_config* kernelConfig, t_log* kernelLogger);


#endif