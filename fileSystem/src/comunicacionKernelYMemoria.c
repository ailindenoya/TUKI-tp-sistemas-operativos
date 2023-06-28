#include "../include/comunicacionKernelYMemoria.h"
#define PATH_FCB "fcb/"

extern t_log* fileSystemLogger;
extern int socketKERNEL;
extern t_list* listaFCBsAbiertos;

void atenderPeticionesDeKernel(){
    // bucle for para atender peticiones, implementar algun switch como en CPU para instrucciones
}

void F_OPEN(char* NombreArchivo){

    char* ruta = concat(PATH_FCB, NombreArchivo);
    int fd = open(ruta, O_RDWR);

    if (fd == -1){
        log_error(fileSystemLogger, "No existe el archivo FCB: %s", NombreArchivo);

        stream_enviar_buffer_vacio(socketKERNEL, HEADER_no_existe_archivo);
        uint8_t respuestaKERNEL = stream_recibir_header(socketKERNEL);
        stream_recibir_buffer_vacio(socketKERNEL);

        if(respuestaKERNEL == HEADER_crear_archivo){
            crearArchivoFCB(NombreArchivo);
        }
        else {
            log_error(fileSystemLogger, "No se recibio respuesta de Kernel de F_OPEN");
            exit(-1);
        }
    }
    t_fcb* fcbAbierto = fcb_config_crear(ruta, fileSystemLogger);
    list_add(listaFCBsAbiertos, (void*) fcbAbierto);

    t_buffer* mensajeOK = buffer_crear();
    buffer_empaquetar_string(mensajeOK, NombreArchivo);
    stream_enviar_buffer(socketKERNEL, HEADER_archivo_abierto, mensajeOK);
    buffer_destruir(mensajeOK);
    free(ruta);
}

void F_TRUNCATE(char* NombreArchivo, uint32_t tamanioNuevo){

    char* ruta = concat(PATH_FCB, NombreArchivo);
    int fd = open(ruta, O_RDWR);

    t_fcb* fcb = encontrarFCB(NombreArchivo);
    uint32_t tamanioViejo = fcb_obtener_tamanio_archivo(fcb);

    fcb_setear_tamanio_archivo(fcb, tamanioNuevo);

    int cantBloques = abs((int) ceil(tamanioNuevo - tamanioViejo / 64));

    if(tamanioNuevo > tamanioViejo){    // Ampliar archivo, asignar más bloques
        // Buscar bloques libres en el bitmap con las funciones de bitarray
        // Agregar cantBloques, poniendo 1 en el bitmap, y actualizar punteros en FCB
    }
    else if (tamanioNuevo < tamanioViejo){  // Reducir, quitar bloques
        // En bitmap poner cantBloques en 0 de ese archivo, duda: buscar los bloques en bloques.dat y borrarlos? actualizar puntero indirecto
        if (tamanioNuevo == 0){
            fcb_setear_puntero_directo(fcb, 0);
            fcb_setear_puntero_indirecto(fcb, 0);
        }     
    }
}