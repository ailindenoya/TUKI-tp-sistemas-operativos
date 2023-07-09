#include "../include/comunicacionKernelYMemoria.h"
#define PATH_FCB "fcb/"

extern t_log* fileSystemLogger;
extern int socketKERNEL;
extern t_list* listaFCBsAbiertos;
extern t_superbloque_config* superbloqueConfig;
extern t_fileSystem_config* fileSystemConfig;

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
                buffer_desempaquetar_string(bufferAux, &parametro2);
                uint32_t tamanio = atoi(parametro2);
                F_TRUNCATE(nombreArchivo, tamanio);
                break;
            case HEADER_F_READ:
                log_info(fileSystemLogger, "Se recibió FREAD");
                t_config* fcb = encontrarFCB(nombreArchivo);
                uint32_t puntero, cantBytes;
                buffer_desempaquetar(bufferAux,&puntero, sizeof(puntero));
                buffer_desempaquetar_string(bufferAux, &parametro3); // cantbytes
                cantBytes = atoi(parametro3); 


                /// manejar logica FREAD TODO


                buffer_empaquetar(bufferAux, &cantBytes, sizeof(cantBytes));
                stream_enviar_buffer(fileSystem_config_obtener_socket_memoria(fileSystemConfig), HEADER_valor_de_registro, bufferAux);
                int respuestaMemoria = stream_stream_recibir_header(fileSystem_config_obtener_socket_memoria(fileSystemConfig));
                if(respuestaMemoria!= HEADER_OK_puede_continuar){
                    log_error(fileSystemLogger, "error al recibir ok de memoria luego de escribir"); 
                    exit(-1);
                }
                stream_recibir_buffer_vacio(fileSystem_config_obtener_socket_memoria(fileSystemConfig));
                break;
            case HEADER_F_WRITE:
                log_info(fileSystemLogger, "Se recibió FREAD");
                t_config* fcb = encontrarFCB(nombreArchivo);
                uint32_t puntero, cantBytes;
                buffer_desempaquetar(bufferAux,&puntero, sizeof(puntero));
                buffer_desempaquetar_string(bufferAux, &parametro3); // cantbytes
                cantBytes = atoi(parametro3); 
                
                // manejar logica FWRITE TODO

                buffer_empaquetar(bufferAux, &cantBytes, sizeof(cantBytes));
                stream_enviar_buffer(fileSystem_config_obtener_socket_memoria(fileSystemConfig), HEADER_valor_de_memoria, bufferAux);
                int respuestaMemoria = stream_stream_recibir_header(fileSystem_config_obtener_socket_memoria(fileSystemConfig));
                if(respuestaMemoria!= HEADER_OK_puede_continuar){
                    log_error(fileSystemLogger, "error al recibir ok de memoria luego de escribir"); 
                    exit(-1);
                }
                stream_recibir_buffer_vacio(fileSystem_config_obtener_socket_memoria(fileSystemConfig));
                break;
            case HEADER_F_CLOSE:
                t_config* fcbABorrar = encontrarFCB(nombreArchivo);
                if(list_remove_element(listaFCBsAbiertos, fcbABorrar) == false){
                    log_error(fileSystemLogger, "error al encontrar FCB a borrar");
                }
                break;
            default:
                log_error(fileSystemLogger, "Error al recibir la instrucción de Kernel");
                break;
        }
        buffer_destruir(bufferAux);
        free(nombreArchivo);
        free(parametro2);
        free(parametro3);
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
    log_info(fileSystemLogger, "Abrir Archivo: %s", NombreArchivo);
    t_buffer* mensajeOK = buffer_crear();
    buffer_empaquetar_string(mensajeOK, NombreArchivo);
    stream_enviar_buffer(socketKERNEL, HEADER_archivo_abierto, mensajeOK);
    buffer_destruir(mensajeOK);
    free(ruta);
}

void F_TRUNCATE(char* NombreArchivo, uint32_t tamanioNuevo){

    char* ruta = concat(PATH_FCB, NombreArchivo);
    // int fd = open(ruta, O_RDWR);
    uint32_t tamanioBloque = superbloque_config_obtener_block_size(superbloqueConfig);  // Para no escribir el getter de block size a cada rato

    t_config* fcb = encontrarFCB(NombreArchivo);

    uint32_t tamanioViejo = config_get_int_value(fcb,"TAMANIO_ARCHIVO");
    char* tam = string_itoa(tamanioNuevo);

    config_set_value(fcb, "TAMANIO_ARCHIVO", tam); 
    config_save_in_file(fcb, ruta);
    
    double tamanioEnBloquesViejo = my_ceil((double) tamanioViejo / tamanioBloque);
    double tamanioEnBloquesNuevo = my_ceil((double) tamanioNuevo / tamanioBloque);

    int cantBloques = abs(tamanioEnBloquesNuevo - tamanioEnBloquesViejo);    // Cantidad de bloques a agregar o quitar
    log_info(fileSystemLogger,"Cantidad de Bloques: %d", cantBloques);
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