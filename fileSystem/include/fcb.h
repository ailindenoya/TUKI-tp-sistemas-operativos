#include <stdlib.h>
#include "fileSystem_config.h"
#include "../../utils/include/config_inicializacion.h"
#include <fcntl.h>
#include <commons/config.h>
#include <sys/mman.h>
#include <inttypes.h>

void crearArchivoFCB(char*);
char* concat(const char*, const char*);

t_config* config_iniciar_fcb(char*);

t_config* encontrarFCB(char*);
void fcb_asignar_bloque(t_config*, uint32_t, uint32_t);
void fcb_quitar_bloque(t_config*, int);
double my_ceil(double);