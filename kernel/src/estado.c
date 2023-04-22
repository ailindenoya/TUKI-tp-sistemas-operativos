#include "../include/estado.h"

#include <stdlib.h>
#include <time.h>

struct t_estado {
    t_nombre_estado nombreEstado;
    t_list* listaProcesos;
    sem_t* semaforoEstado;
    pthread_mutex_t* mutexEstado;
};




int list_get_index(t_list* list, bool (*cutting_condition)(void*, void*), void* target) {
    for (int i = 0; i < list_size(list); i++) {
        void* temp = list_get(list, i);
        if (cutting_condition(temp, target)) { // NO VA -> PONER CONDICION DE OTRA MANERA, SOLO COMPARA EL PID DEL PCB 
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
    uint32_t index = list_get_index(estado_get_list(self), pcb_es_este_pcb_por_pid, targetPcb);
    if (index != -1) {
        pcb = list_remove(estado_get_list(self), index);
    }
    return pcb;
}

t_pcb* estado_desencolar_primer_pcb(t_estado* self) {
    return list_remove(estado_get_list(self), 0);
}

t_list* estado_get_list(t_estado* self) {
    return self->listaProcesos;
}

sem_t* estado_get_sem(t_estado* self) {
    return self->semaforoEstado;
}

pthread_mutex_t* estado_get_mutex(t_estado* self) {
    return self->mutexEstado;
}