#include <errno.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/socket.h>

#include "../../utils/include/flagsParaCommons.h"
#include "../../utils/include/conexiones.h"
#include "../../utils/include/stream.h"

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>

typedef struct t_fileSystem_config t_fileSystem_config;

t_fileSystem_config* fileSystem_config_crear(char*,t_log*);
void fileSystem_destruir(t_fileSystem_config*, t_log*);
 void fileSystem_config_iniciar(void*, t_config*);
void fileSystem_destruir(t_fileSystem_config* , t_log* );
void fileSystem_config_destruir(t_fileSystem_config* self);
char* fileSystem_config_obtener_ip_memoria(t_fileSystem_config*);
char* fileSystem_config_obtener_puerto_memoria(t_fileSystem_config*);
char* fileSystem_config_obtener_puerto_escucha(t_fileSystem_config*);
char* fileSystem_config_obtener_path_superbloque(t_fileSystem_config*);
char* fileSystem_config_obtener_path_bitmap(t_fileSystem_config*);
char* fileSystem_config_obtener_path_bloques(t_fileSystem_config*);
char* fileSystem_config_obtener_path_fcb(t_fileSystem_config*);
int fileSystem_config_obtener_retardo_acceso_bloque(t_fileSystem_config*);
int fileSystem_config_setear_socket_memoria(t_fileSystem_config* self, int socket);