#ifndef FCB_H_INCLUDED
#define FCB_H_INCLUDED

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
void fcb_asignar_bloque(t_config*, uint32_t, uint32_t, char*);
void fcb_quitar_bloque(t_config*, int);
char* leerBloqueDirecto(uint32_t, uint32_t, uint32_t, char*);
char* leerBloques(t_config*, uint32_t, uint32_t, char*);
void escribirEnBloqueDirecto(uint32_t, uint32_t, uint32_t, char*, char*);
void escribirBloques(t_config*, uint32_t, uint32_t, char*, char*);
double my_ceil(double);
int min(int, int);

#endif