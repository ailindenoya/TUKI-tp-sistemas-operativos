#include "../../utils/include/flagsParaCommons.h"
#include <commons/collections/list.h>
#include "../include/kernel_config.h"
#include <stdio.h>
#include "../include/pcb.h"
struct t_archivo_tabla{
    uint32_t pid;
    char* nombreArchivo;
    t_list* colaProcesos;
};

struct t_archivo_tabla_proceso{
    char* nombreArchivo;
    uint32_t puntero;
};

typedef struct t_archivo_tabla t_archivo_tabla;
typedef struct t_archivo_tabla_proceso t_archivo_tabla_proceso;
t_archivo_tabla* crearEntradaEnTabla(uint32_t, char*);
t_archivo_tabla_proceso* crearEntradaEnTablaProceso(char*);

uint32_t t_archivo_tabla_obtener_pid(t_archivo_tabla* self);
char* t_archivo_tabla_obtener_nombre_archivo(t_archivo_tabla* self);
t_list* t_archivo_tabla_obtener_cola_procesos(t_archivo_tabla* self);
void t_archivo_tabla_setear_pid(t_archivo_tabla* self, uint32_t pidNuevo);
void t_archivo_tabla_setear_nombre_archivo(t_archivo_tabla* self, char* NombreNuevo);
void t_archivo_tabla_actualizar_cola_procesos(t_archivo_tabla* self, t_pcb* pcb);

void t_archivo_tabla_proceso_setear_puntero(t_archivo_tabla_proceso*, uint32_t);
char* t_archivo_tabla_proceso_obtener_nombre_archivo(t_archivo_tabla_proceso*);
uint32_t t_archivo_tabla_proceso_obtener_puntero(t_archivo_tabla_proceso*);
