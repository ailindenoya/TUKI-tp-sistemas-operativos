#include "../include/comunicacionKernelYMemoria.h"
#define PATH_FCB "fcb/"

extern t_log* fileSystemLogger;
extern int socketKERNEL;
extern t_list* listaFCBsAbiertos;
extern t_superbloque_config* superbloqueConfig;


void dispatch_FS_peticiones_de_Kernel(void){    // Completar con demás instrucciones

    for(;;){
        char* nombreArchivo;
        char* parametro2;
        //char* parametro3;

        t_buffer* bufferAux = buffer_crear();
        stream_recibir_buffer(socketKERNEL, bufferAux);
        buffer_desempaquetar_string(bufferAux, &nombreArchivo);

        uint8_t kernelRespuesta = stream_recibir_header(socketKERNEL);

        switch(kernelRespuesta){
            case HEADER_F_OPEN:
                buffer_destruir(bufferAux);
                F_OPEN(nombreArchivo);
                break;
            case HEADER_F_TRUNCATE:
                buffer_desempaquetar_string(bufferAux, &parametro2);
                uint32_t tamanio = atoi(parametro2);
                buffer_destruir(bufferAux);

                F_TRUNCATE(nombreArchivo, tamanio);
                break;
            default:
                log_error(fileSystemLogger, "Error al recibir la instrucción de Kernel");
                break;
        }
    }
}

void atenderPeticionesDeKernel(void){
    log_info(fileSystemLogger, "Listo para atender peticiones de Kernel");
    dispatch_FS_peticiones_de_Kernel();
}

void F_OPEN(char* NombreArchivo){

    char* ruta = concat(PATH_FCB, NombreArchivo);
    int fd = open(ruta, O_RDWR);

    if (fd == -1){
        log_error(fileSystemLogger, "No existe el archivo FCB: %s", NombreArchivo);

        uint8_t respuestaKERNEL = stream_recibir_header(socketKERNEL);
        stream_enviar_buffer_vacio(socketKERNEL, HEADER_no_existe_archivo);
        
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

    fcb_setear_tamanio_archivo(fcb, tamanioNuevo); // 65 a 63 2 bloques a 1   50 a 40 1 bloque 1 bloque


    int cantBloques = abs(ceil(tamanioNuevo / superbloque_config_obtener_block_size(superbloqueConfig)) - ceil(tamanioViejo / superbloque_config_obtener_block_size(superbloqueConfig)));

    if (cantBloques == 0){

    }
    else {
        if(tamanioNuevo > tamanioViejo){            
            agregarBloques(cantBloques, fcb);
        }
        else if (tamanioNuevo < tamanioViejo){
            quitarBloques(cantBloques, fcb);
            // Reducir, quitar bloques
            // En bitmap poner cantBloques en 0 de ese archivo, duda: buscar los bloques en bloques.dat y borrarlos? actualizar puntero indirecto
            if (tamanioNuevo == 0){
                fcb_setear_puntero_directo(fcb, 0);
                fcb_setear_puntero_indirecto(fcb, 0);
            }     
        }
    }
    log_info(fileSystemLogger, "Truncar Archivo: %s - Tamaño: %d", NombreArchivo, tamanioNuevo);
}