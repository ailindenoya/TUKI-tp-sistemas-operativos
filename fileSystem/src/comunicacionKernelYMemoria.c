#include "../include/comunicacionKernelYMemoria.h"
#define PATH_FCB "fcb/"

extern t_log* fileSystemLogger;
extern int socketKERNEL;
extern t_list* listaFCBsAbiertos;
extern t_superbloque_config* superbloqueConfig;




void dispatch_FS_peticiones_de_Kernel(void){    // Completar con demás instrucciones

    for(;;){
        char* nombreArchivo = malloc(sizeof(*nombreArchivo));
        char* parametro2 = malloc(sizeof(*parametro2));
        char* parametro3 = malloc(sizeof(*parametro3));

        t_buffer* bufferAux = buffer_crear();
        uint8_t kernelRespuesta = stream_recibir_header(socketKERNEL);
        stream_recibir_buffer(socketKERNEL, bufferAux);
        buffer_desempaquetar_string(bufferAux, &nombreArchivo);

        switch(kernelRespuesta){
            case HEADER_F_OPEN:
                F_OPEN(nombreArchivo);
                break;
            case HEADER_F_TRUNCATE:
                log_info(fileSystemLogger, "apertura de archivo en ftrunc: %s", nombreArchivo);
                log_info(fileSystemLogger, "a empaquetar");
                buffer_desempaquetar_string(bufferAux, &parametro2);
                log_info(fileSystemLogger, "empaqueto string: %s", parametro2);
                uint32_t tamanio = atoi(parametro2);
                F_TRUNCATE(nombreArchivo, tamanio);
                break;
            case HEADER_F_READ:
                log_info(fileSystemLogger, "se recibio FREAD");
                buffer_desempaquetar_string(bufferAux, &parametro2);
                buffer_desempaquetar_string(bufferAux, &parametro3); 
            
            default:
                log_error(fileSystemLogger, "Error al recibir la instrucción de Kernel");
                break;
        }
        buffer_destruir(bufferAux);
        free(nombreArchivo);
        free(parametro2);
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

        stream_enviar_buffer_vacio(socketKERNEL, HEADER_no_existe_archivo);

        uint8_t respuestaKERNEL = stream_recibir_header(socketKERNEL);
        //2)


        if(respuestaKERNEL == HEADER_crear_archivo){
            crearArchivoFCB(NombreArchivo);

        }
        else {
            log_error(fileSystemLogger, "No se recibio respuesta de Kernel de F_OPEN");
            exit(-1);
        }
    }
    t_config* fcbAbierto = config_iniciar_fcb(ruta);
    list_add(listaFCBsAbiertos, fcbAbierto);
    log_info(fileSystemLogger, "apertura de archivo: %s", NombreArchivo);
    t_buffer* mensajeOK = buffer_crear();
    buffer_empaquetar_string(mensajeOK, NombreArchivo);
    stream_enviar_buffer(socketKERNEL, HEADER_archivo_abierto, mensajeOK);
    buffer_destruir(mensajeOK);
    free(ruta);
}

void F_TRUNCATE(char* NombreArchivo, uint32_t tamanioNuevo){

    char* ruta = concat(PATH_FCB, NombreArchivo);
   // int fd = open(ruta, O_RDWR);

    t_config* fcb = encontrarFCB(NombreArchivo);
    uint32_t tamanioViejo = config_get_int_value(fcb,"TAMANIO_ARCHIVO");
    char* tam = string_itoa(tamanioNuevo);
    config_set_value(fcb, "TAMANIO_ARCHIVO", tam); // 65 a 63 2 bloques a 1   50 a 40 1 bloque 1 bloque
    config_save_in_file(fcb, ruta);
    
    uint32_t cantBloques = abs(ceil(tamanioNuevo / superbloque_config_obtener_block_size(superbloqueConfig)) 
    - ceil(tamanioViejo / superbloque_config_obtener_block_size(superbloqueConfig)));
// cant de bloques a agregar o quitar. 
    log_info(fileSystemLogger, "cant de bloques: %d", cantBloques);
    log_info(fileSystemLogger, "tamNUevo-: %d", tamanioNuevo);
    log_info(fileSystemLogger, "tamViejo: %d", tamanioViejo);
    if (cantBloques == 0){
        
    }
    else {
        if(tamanioNuevo > tamanioViejo){            
            agregarBloques(cantBloques, fcb, ruta );
        }
        else if (tamanioNuevo < tamanioViejo){
            quitarBloques(cantBloques, fcb);
            if (tamanioNuevo == 0){
                config_set_value(fcb, "TAMANIO_ARCHIVO", "0");
                config_set_value(fcb, "PUNTERO_DIRECTO", "-1");
                config_set_value(fcb, "PUNTERO_INDIRECTO", "-1");
            }     
        }
    }
    log_info(fileSystemLogger, "Truncar Archivo: %s - Tamaño: %d", NombreArchivo, tamanioNuevo);
}