extern int socketFILESYSTEM;
#include "../include/comunicacionFileSystem.h"
extern t_log* kernelLogger;
//extern t_list* tablaArchivosAbiertos;

struct t_archivo_tabla{
    __uint32_t pid;
    char* nombreArchivo;
    t_list* colaProcesos;
};

__uint32_t t_archivo_tabla_obtener_pid(t_archivo_tabla* self){
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
void t_archivo_tabla_actualizar_cola_procesos(t_archivo_tabla* self, uint32_t pidAgregar){
    list_add(self->colaProcesos,pidAgregar);
}