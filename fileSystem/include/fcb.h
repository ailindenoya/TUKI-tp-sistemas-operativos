#include <stdlib.h>
#include "fileSystem_config.h"
#include "../../utils/include/config_inicializacion.h"
#include <fcntl.h>
#include <commons/config.h>
#include <sys/mman.h>


typedef struct t_fcb t_fcb;

t_fcb* fcb_crear(char*);

char* fcb_obtener_nombre_archivo(t_fcb*);
uint32_t fcb_obtener_tamanio_archivo(t_fcb*);
uint32_t fcb_obtener_puntero_directo(t_fcb*);
uint32_t fcb_obtener_puntero_indirecto(t_fcb*);
void fcb_setear_tamanio_archivo(t_fcb*, uint32_t);
void fcb_setear_puntero_directo(t_fcb* , uint32_t );
void fcb_setear_puntero_indirecto(t_fcb*, uint32_t);

void crearArchivoFCB(char*);
char* concat(const char*, const char*);

void fcb_config_iniciar(void*, t_config*);
t_fcb* fcb_config_crear(char*, t_log*);
int config_iniciar_fcb(void* config, char* pathAlConfig, t_log* logger,void (*config_initializer)(void* moduleConfig, t_config* configTemp));

t_fcb* encontrarFCB(char*);