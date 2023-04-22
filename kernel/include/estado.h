#ifndef ESTADOS_H_INCLUDED
#define ESTADOS_H_INCLUDED

#include <commons/collections/list.h>
#include <semaphore.h>

#include "pcb.h"

typedef struct t_estado t_estado;

typedef enum {
    NEW,
    READY,
    EXEC,
    EXIT,
    BLOCKED,
    SUSPENDED_READY,
    SUSPENDED_BLOCKED,
    PCBS_ESPERANDO_PARA_IO,
} t_nombre_estado;


int list_get_index(t_list *, bool (*cutting_condition)(void *, void *), void *target); 
// NO VA -> PONER CONDICION DE OTRA MANERA, SOLO COMPARA EL PID DEL PCB 
void intervalo_de_pausa(uint32_t duracionEnMilisegundos);

pthread_mutex_t *estado_obtener_mutex(t_estado *);
sem_t *estado_obtener_sem(t_estado *);
t_estado *estado_crear(t_nombre_estado);
t_list *estado_obtener_list(t_estado *);
t_pcb *estado_desencolar_primer_pcb(t_estado *);
t_pcb *estado_remover_pcb_de_cola(t_estado *, t_pcb *);
void estado_destruir(t_estado *);




#endif