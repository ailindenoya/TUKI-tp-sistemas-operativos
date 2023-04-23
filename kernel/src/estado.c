#include "../include/estado.h"

#include <stdlib.h>
#include <time.h>

struct t_estado {
    t_nombre_estado nombreEstado;
    t_list* listaProcesos;
    sem_t* semaforoEstado;
    pthread_mutex_t* mutexEstado;
};




int obtener_indice_de_pcb(t_list* list, t_pcb* otroPCB) {
    for (int i = 0; i < list_size(list); i++) {
        t_pcb* unPCB = list_get(list, i);
        if (pcb_get_pid(unPCB) == pcb_get_pid(otroPCB)) { 
            return i;
        }
    }
    return -1;
}

void intervalo_de_pausa(uint32_t duracionEnMilisegundos) {
    const uint32_t SECS_MILISECS = 1000;
    const uint32_t MILISECS_NANOSECS = 1000000;
    struct timespec timeSpec;
    timeSpec.tv_sec = duracionEnMilisegundos / SECS_MILISECS;
    timeSpec.tv_nsec = (duracionEnMilisegundos % SECS_MILISECS) * MILISECS_NANOSECS;
    nanosleep(&timeSpec, &timeSpec);
}

t_estado* estado_create(t_nombre_estado nombre) {
    t_estado* self = malloc(sizeof(*self));
    self->listaProcesos = list_create();
    self->mutexEstado = malloc(sizeof(*(self->mutexEstado)));
    pthread_mutex_init(self->mutexEstado, NULL);
    self->nombreEstado = nombre;
    self->semaforoEstado = malloc(sizeof(*(self->semaforoEstado)));
    sem_init(self->semaforoEstado, 0, 0);
    return self;
}

void estado_destroy(t_estado* self) {
    if (list_is_empty(self->listaProcesos)) {
        list_destroy(self->listaProcesos);
    } else {
        list_destroy_and_destroy_elements(self->listaProcesos, (void*)pcb_destroy);
    }
    pthread_mutex_destroy(self->mutexEstado);
    sem_destroy(self->semaforoEstado);
    free(self->semaforoEstado);
    free(self->mutexEstado);
    free(self);
}

t_pcb* estado_remover_pcb_de_cola(t_estado* self, t_pcb* targetPcb) {
    t_pcb* pcb = NULL;
    uint32_t index = obtener_indice_de_pcb(estado_obtener_lista(self), targetPcb);
    if (index != -1) {
        pcb = list_remove(estado_obtener_lista(self), index);
    }
    return pcb;
}

t_pcb* estado_desencolar_primer_pcb(t_estado* self) {
    return list_remove(estado_obtener_lista(self), 0);
}

t_list* estado_obtener_lista(t_estado* self) {
    return self->listaProcesos;
}

sem_t* estado_obtener_sem(t_estado* self) {
    return self->semaforoEstado;
}

pthread_mutex_t* estado_obtener_mutex(t_estado* self) {
    return self->mutexEstado;
}

t_pcb* estado_desencolar_primer_pcb_con_semaforo(t_estado* self) {
    pthread_mutex_lock(estado_get_mutex(self));
    t_pcb* pcb = estado_desencolar_primer_pcb(self);
    pthread_mutex_unlock(estado_get_mutex(self));
    return pcb;
}

void estado_encolar_pcb_con_semaforo(t_estado* estadoDest, t_pcb* targetPcb) {
    pthread_mutex_lock(estado_get_mutex(estadoDest));
    list_add(estado_get_list(estadoDest), targetPcb);
    pthread_mutex_unlock(estado_get_mutex(estadoDest));
}

bool estado_contiene_pcb_con_semaforo(t_estado* self, t_pcb* targetPcb) {
    pthread_mutex_lock(estado_get_mutex(self));
    bool contienePCB = false;
    uint32_t index = obtener_indice_de_pcb(estado_get_list(self), targetPcb);
    if (index != -1) {
        contienePCB = true;
    }
    pthread_mutex_unlock(estado_get_mutex(self));
    return contienePCB;
}

t_pcb* estado_remover_pcb_de_cola_con_semaforo(t_estado* self, t_pcb* targetPcb) {
    pthread_mutex_lock(estado_get_mutex(self));
    t_pcb* pcb = estado_remover_pcb_de_cola(self, targetPcb);
    pthread_mutex_unlock(estado_get_mutex(self));
    return pcb;
}