#include "../include/comunicacionKernelYMemoria.h"
#include "../../utils/include/stream.h"
#include <commons/collections/list.h>
#include <semaphore.h>
#define PATH_FCB "fcb/"

extern sem_t sePuedeCompactar;  
extern int socketMEMORIA;
extern uint32_t tamanioBloque;
extern t_log* fileSystemLogger;
extern t_list* listaFCBsAbiertos;
extern t_superbloque_config* superbloqueConfig;
extern t_fileSystem_config* fileSystemConfig;

extern void* bitmap;
extern void* bloques;

void dispatch_FS_peticiones_de_Kernel(void){    // Completar con demás instrucciones
    int socketKERNEL = fileSystem_config_obtener_socket_kernel_peticiones(fileSystemConfig);
    int socketKERNELDESBLOQUEOS = fileSystem_config_obtener_socket_kernel_desbloqueos(fileSystemConfig);
    
    for(;;){
        char* nombreArchivo = malloc(sizeof(*nombreArchivo));
        char* parametro2 = malloc(sizeof(*parametro2));
        char* parametro3 = malloc(sizeof(*parametro3));
        t_config* fcb;
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
                t_buffer* bufferDesbloqueoTruncate = buffer_crear();
                buffer_empaquetar_string(bufferDesbloqueoTruncate, nombreArchivo);
                stream_enviar_buffer(socketKERNELDESBLOQUEOS, HEADER_desbloquear_proceso, bufferDesbloqueoTruncate);
                buffer_destruir(bufferDesbloqueoTruncate);
                break;
            case HEADER_F_READ:
                sem_wait(&sePuedeCompactar);
                log_info(fileSystemLogger, "Se recibió FREAD");
                fcb = encontrarFCB(nombreArchivo);
                uint32_t punteroF_READ, cantBytesF_READ, direccionLogicaDeFREAD;
                buffer_desempaquetar(bufferAux,&punteroF_READ, sizeof(punteroF_READ));
                buffer_desempaquetar_string(bufferAux, &parametro3); // cantbytes
                buffer_desempaquetar(bufferAux, &direccionLogicaDeFREAD, sizeof(direccionLogicaDeFREAD));
                cantBytesF_READ = atoi(parametro3); 
                char* datosLeidos = F_READ(fcb, cantBytesF_READ, punteroF_READ);
                log_info(fileSystemLogger, "Leer Archivo: %s - Puntero: %d - Memoria: %d - Tamaño: %d", nombreArchivo, punteroF_READ, direccionLogicaDeFREAD, cantBytesF_READ);
                buffer_empaquetar(bufferAux, &cantBytesF_READ, sizeof(cantBytesF_READ));
                buffer_empaquetar(bufferAux, datosLeidos, sizeof(*datosLeidos)*cantBytesF_READ);
                stream_enviar_buffer(fileSystem_config_obtener_socket_memoria(fileSystemConfig), HEADER_valor_de_registro, bufferAux);
                int respuestaMemoriaF_READ = stream_recibir_header(fileSystem_config_obtener_socket_memoria(fileSystemConfig));
                if(respuestaMemoriaF_READ!= HEADER_OK_puede_continuar){
                    log_error(fileSystemLogger, "error al recibir ok de memoria luego de escribir"); 
                    exit(-1);
                }
                stream_recibir_buffer_vacio(fileSystem_config_obtener_socket_memoria(fileSystemConfig));
                free(datosLeidos);
                t_buffer* bufferDesbloqueoFREAD = buffer_crear();
                buffer_empaquetar_string(bufferDesbloqueoFREAD, nombreArchivo);
                stream_enviar_buffer(socketKERNELDESBLOQUEOS, HEADER_desbloquear_proceso, bufferDesbloqueoFREAD);
                buffer_destruir(bufferDesbloqueoFREAD);
                sem_post(&sePuedeCompactar);
                break;
            case HEADER_F_WRITE:
                sem_wait(&sePuedeCompactar);    
                log_info(fileSystemLogger, "Se recibió F_WRITE");
                fcb = encontrarFCB(nombreArchivo);
                uint32_t punteroF_WRITE, cantBytesF_WRITE, dirLogica;
                buffer_desempaquetar(bufferAux,&punteroF_WRITE, sizeof(punteroF_WRITE));
                buffer_desempaquetar_string(bufferAux, &parametro3); // cantbytes
                buffer_desempaquetar(bufferAux, &dirLogica, sizeof(dirLogica));
                cantBytesF_WRITE = atoi(parametro3); 
                buffer_empaquetar(bufferAux, &cantBytesF_WRITE, sizeof(cantBytesF_WRITE));
                stream_enviar_buffer(fileSystem_config_obtener_socket_memoria(fileSystemConfig), HEADER_valor_de_memoria, bufferAux);
                int respuestaMemoriaF_WRITE = stream_recibir_header(fileSystem_config_obtener_socket_memoria(fileSystemConfig));
                if(respuestaMemoriaF_WRITE!= HEADER_OK_puede_continuar){
                    log_error(fileSystemLogger, "error al recibir ok de memoria luego de escribir %d", respuestaMemoriaF_WRITE); 
                    exit(-1);
                }
                t_buffer* bufferValorDeMemoria = buffer_crear();
                char* informacion = malloc(sizeof(*informacion)*cantBytesF_WRITE);
                int respuestaValorDeMemoria = stream_recibir_header(fileSystem_config_obtener_socket_memoria(fileSystemConfig));
                if(respuestaValorDeMemoria!= HEADER_valor_de_memoria){
                    log_error(fileSystemLogger, "error al recibir valor de memoria %d", respuestaValorDeMemoria); 
                    exit(-1);
                }
                stream_recibir_buffer(fileSystem_config_obtener_socket_memoria(fileSystemConfig), bufferValorDeMemoria);   
                buffer_desempaquetar(bufferValorDeMemoria, informacion, sizeof(*informacion)*cantBytesF_WRITE);
                buffer_destruir(bufferValorDeMemoria);

                F_WRITE(fcb, cantBytesF_WRITE, punteroF_WRITE, informacion);
                log_info(fileSystemLogger, "Leer Archivo: %s - Puntero: %d - Memoria: %d - Tamaño: %d", nombreArchivo, punteroF_READ,dirLogica,cantBytesF_READ);

                t_buffer* bufferDesbloqueoFWRITE = buffer_crear();
                buffer_empaquetar_string(bufferDesbloqueoFWRITE, nombreArchivo);
                stream_enviar_buffer(socketKERNELDESBLOQUEOS, HEADER_desbloquear_proceso, bufferDesbloqueoFWRITE);
                buffer_destruir(bufferDesbloqueoFWRITE);
                sem_post(&sePuedeCompactar);
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
    int socketKERNEL = fileSystem_config_obtener_socket_kernel_peticiones(fileSystemConfig);

    char* ruta = concat(PATH_FCB, NombreArchivo);
    int fd = open(ruta, O_RDWR);

    if (fd == -1){
        log_info(fileSystemLogger, "No existe el archivo FCB: %s", NombreArchivo);

        stream_enviar_buffer_vacio(socketKERNEL, HEADER_no_existe_archivo);

        uint8_t respuestaKERNEL = stream_recibir_header(socketKERNEL);
        
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

    log_info(fileSystemLogger, "Ejecutando: F_TRUNCATE - Archivo: %s", NombreArchivo);
    char* ruta = concat(PATH_FCB, NombreArchivo);

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
        // El tamaño del truncate no implica asignar o quitar bloques
    }
    else {
        if(tamanioNuevo > tamanioViejo){            
            agregarBloques(cantBloques, fcb, tamanioViejo, ruta);
            config_save_in_file(fcb, ruta);
        }
        else if (tamanioNuevo < tamanioViejo){
            quitarBloques(cantBloques, fcb, tamanioViejo, ruta);
            config_save_in_file(fcb, ruta);
        }
        else if (tamanioNuevo == 0){
            config_set_value(fcb, "TAMANIO_ARCHIVO", "0");
            config_set_value(fcb, "PUNTERO_DIRECTO", "-1");
            config_set_value(fcb, "PUNTERO_INDIRECTO", "-1");
        }     
    }
    log_info(fileSystemLogger, "Truncar Archivo: %s - Tamaño: %d", NombreArchivo, tamanioNuevo);
    config_save_in_file(fcb, ruta);
}


char* F_READ(t_config* fcb, uint32_t cantBytes, uint32_t puntero){
    char* datos = malloc(cantBytes);
    char* nombreArchivo = config_get_string_value(fcb, "NOMBRE_ARCHIVO");
    log_info(fileSystemLogger, "Ejecutando: F_READ - Archivo: %s - Bytes: %d - Puntero: %d", nombreArchivo, cantBytes, puntero);

    datos = leerBloques(fcb, cantBytes, puntero, nombreArchivo);
    return datos;
    
}


void F_WRITE(t_config* fcb, uint32_t cantBytes, uint32_t puntero, char* informacion){
    char* nombreArchivo = config_get_string_value(fcb, "NOMBRE_ARCHIVO");
    uint32_t tamanioArchivo = config_get_int_value(fcb, "TAMANIO_ARCHIVO");
    if(puntero + cantBytes > tamanioArchivo){
        log_error(fileSystemLogger, "Error: Posicion del puntero en el archivo + cantidad de bytes a escribir es mayor al tamanio del archivo");
        exit(-1);
    }
    
    escribirBloques(fcb, cantBytes, puntero, nombreArchivo, informacion);
}