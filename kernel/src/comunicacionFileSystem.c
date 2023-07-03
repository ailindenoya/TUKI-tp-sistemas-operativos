extern int socketFILESYSTEM;
#include "../include/comunicacionFileSystem.h"
extern t_log* kernelLogger;
extern t_list* tablaArchivosAbiertos;

t_archivo_tabla* crearEntradaEnTabla(uint32_t pid, char* NombreArchivo){
    t_archivo_tabla* aux = malloc(sizeof(*aux));
    aux->pid = pid;
    aux->nombreArchivo = NombreArchivo;
    aux->colaProcesos = list_create();
    return aux;
}

t_archivo_tabla_proceso* crearEntradaEnTablaProceso(char* NombreArchivo){
    t_archivo_tabla_proceso* aux = malloc(sizeof(*aux));
    aux->nombreArchivo = NombreArchivo;
    aux->puntero = 0;
    return aux;
}

uint32_t t_archivo_tabla_obtener_pid(t_archivo_tabla* self){
    return self->pid;
}
char* t_archivo_tabla_obtener_nombre_archivo(t_archivo_tabla* self){
    return self->nombreArchivo;
}
t_list* t_archivo_tabla_obtener_cola_procesos(t_archivo_tabla* self){
    return self->colaProcesos;
}
void t_archivo_tabla_setear_pid(t_archivo_tabla* self, uint32_t pidNuevo){
    self->pid = pidNuevo;
}
void t_archivo_tabla_setear_nombre_archivo(t_archivo_tabla* self, char* NombreNuevo){
    self->nombreArchivo = NombreNuevo;
}

void t_archivo_tabla_actualizar_cola_procesos(t_archivo_tabla* self, t_pcb* pcb){
    list_add(self->colaProcesos,(void*) pcb);
}
void t_archivo_tabla_proceso_setear_puntero(t_archivo_tabla_proceso* self, uint32_t puntero){
    self->puntero = puntero;
}
uint32_t t_archivo_tabla_proceso_obtener_puntero(t_archivo_tabla_proceso* self){
    return self->puntero;
}
char* t_archivo_tabla_proceso_obtener_nombre_archivo(t_archivo_tabla_proceso* self){
    return self->nombreArchivo;
}