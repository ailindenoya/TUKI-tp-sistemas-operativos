#include "../include/comunicacionKernelYMemoria.h"
#define PATH_FCB "../fcb/"

extern t_log* fileSystemLogger;
extern int socketKERNEL;


void F_OPEN(char* NombreArchivo){

    int fd = open(strcat(PATH_FCB, NombreArchivo), O_RDWR);

    if (fd == -1){
        log_error(fileSystemLogger, "No existe el archivo FCB: %s", NombreArchivo);
        crearArchivoFCB(NombreArchivo);
    }

    t_buffer* mensajeOK = buffer_crear();
    buffer_empaquetar_string(mensajeOK, "OK");
    stream_enviar_buffer(socketKERNEL, HEADER_existe_archivo, mensajeOK);
    buffer_destruir(mensajeOK);

    ;
}