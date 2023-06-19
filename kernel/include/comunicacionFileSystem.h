#include <stdio.h>
#include "../../utils/include/flagsParaCommons.h"
#include <commons/collections/list.h>
#include "../include/kernel_config.h"

typedef struct t_archivo_tabla t_archivo_tabla;
__uint32_t t_archivo_tabla_obtener_pid(t_archivo_tabla* self);
char* t_archivo_tabla_obtener_nombre_archivo(t_archivo_tabla* self);
t_list* t_archivo_tabla_obtener_cola_procesos(t_archivo_tabla* self);
void t_archivo_tabla_setear_pid(t_archivo_tabla* self, uint32_t pidNuevo);
void t_archivo_tabla_setear_nombre_archivo(t_archivo_tabla* self, char* NombreNuevo);
void t_archivo_tabla_actualizar_cola_procesos(t_archivo_tabla* self, uint32_t pidAgregar);
