#ifndef PLANIFICADOR_H_INCLUDED
#define PLANIFICADOR_H_INCLUDED

#include "kernel_config.h"
#include "pcb.h"
#include "estado.h"
#include <pthread.h>
#include "comunicacionCPU.h"
#include <time.h>
#include "comunicacionFileSystem.h"


void loggear_cambio_estado(const char *prev, const char *post, int pid);
uint32_t obtener_siguiente_pid(void);
void* encolar_en_new_nuevo_pcb_entrante(void*);
void iniciar_planificadores(void);
bool encontrarArchivoEnTabla(char*);


t_pcb* iniciar_fifo(t_estado* estado);

double siguiente_estimacion(double realAnterior, double estimacionAnterior) ;

void actualizar_pcb_por_fin_de_rafaga(t_pcb* pcb);

double response_ratio(double estimacionDeProxRafaga, double tiempoEsperandoEnReady);

t_pcb* mayor_response_ratio(t_pcb* unPcb, t_pcb* otroPcb);

t_pcb* iniciar_HRRN(t_estado* estado, double alfa) ;
#endif