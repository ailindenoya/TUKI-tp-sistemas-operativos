#include <stdlib.h>
#include "fileSystem_config.h"
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "fcb.h"

void cargarBitMap(void);
void limpiarPosiciones(t_bitarray* unEspacio, int posicionInicial, int tamanioProceso);
void agregarBloques(int, t_config*, uint32_t, char*);
void quitarBloques(int, t_config*, uint32_t, char*);
void cargarArchivoDeBloques(void);
uint32_t buscarBloqueLibre(void);