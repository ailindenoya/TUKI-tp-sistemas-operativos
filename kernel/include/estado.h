#ifndef ESTADOS_H_INCLUDED
#define ESTADOS_H_INCLUDED

#include <commons/collections/list.h>
#include <semaphore.h>

#include <pcb.h>

typedef struct t_estado t_estado;

typedef enum {
    NEW,
    READY,
    EXEC,
    EXIT,
    BLOCKED,
    //PCBS_ESPERANDO_PARA_IO
} t_nombre_estado;


int obtener_indice_de_pcb(t_list *,  t_pcb* );
void intervalo_de_pausa(uint32_t duracionEnMilisegundos);

pthread_mutex_t *estado_obtener_mutex(t_estado *);
sem_t *estado_obtener_sem(t_estado *);
t_estado *estado_crear(t_nombre_estado);
t_list *estado_obtener_lista(t_estado *);
t_pcb *estado_desencolar_primer_pcb(t_estado *);
t_pcb *estado_remover_pcb_de_cola(t_estado *, t_pcb *);
void estado_destruir(t_estado *);
t_pcb* estado_desencolar_primer_pcb_con_semaforo(t_estado* self);
void estado_encolar_pcb_con_semaforo(t_estado* estadoDest, t_pcb* targetPcb);
bool estado_contiene_pcb_con_semaforo(t_estado* self, t_pcb* targetPcb);
t_pcb* estado_remover_pcb_de_cola_con_semaforo(t_estado* self, t_pcb* targetPcb);



#endif