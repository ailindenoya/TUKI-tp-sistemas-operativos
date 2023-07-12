#define _GNU_SOURCE
#include "../include/fcb.h"
#include "../include/bitmap.h"
#define PATH_FCB "fcb/"
#define INTMAX_MAX_P1 ((INTMAX_MAX/2 + 1)*2.0)

extern uint32_t tamanioBloque;
extern t_log* fileSystemLogger;
extern t_list* listaFCBsAbiertos;
extern t_superbloque_config* superbloqueConfig;
extern t_fileSystem_config* fileSystemConfig;

extern t_bitarray* bitmapBitarray;
extern int fdBitmap;
extern int fdBloques;
extern void* bitmap;
extern void* bloques;

double my_ceil(double x) {
  if (x >= INTMAX_MAX_P1) {
    return x;
  }
  if (x < INTMAX_MIN) {
    return x;
  }

  intmax_t i = (intmax_t) x;      // this rounds towards 0
  if (i < 0 || x == i) return i;  // negative x is already rounded up.
  return i + 1.0;
}

int min(int x, int y)
{
  return (x < y) ? x : y;
}

void fcb_asignar_bloque(t_config* fcb, uint32_t bloque, uint32_t bloquesAsignadosEnPunteroIndirecto){
    if (config_get_int_value(fcb,"PUNTERO_DIRECTO") == -1){  
        char* bloq = string_itoa(bloque);
        config_set_value(fcb, "PUNTERO_DIRECTO",bloq);
        return;
    }
    uint32_t punteroIndirecto = config_get_int_value(fcb,"PUNTERO_INDIRECTO");

    if (punteroIndirecto == -1){   //HAY QUE ASIGNAR OTRO BLOQUE  aparte del asignado a puntero directo
        char* bloq = string_itoa(bloque);   
        config_set_value(fcb, "PUNTERO_INDIRECTO",bloq);
        uint32_t blqueNuevoAAsignar = buscarBloqueLibre();
        fcb_asignar_bloque(fcb,blqueNuevoAAsignar, bloquesAsignadosEnPunteroIndirecto);
        return;
    }
    int posicionBloqueIndirecto = punteroIndirecto * tamanioBloque;
    char* nombreArchivo = config_get_string_value(fcb, "NOMBRE_ARCHIVO");

    sleep(fileSystem_config_obtener_retardo_acceso_bloque(fileSystemConfig)/1000);
    memcpy(bloques + posicionBloqueIndirecto + bloquesAsignadosEnPunteroIndirecto * 4, &bloque, sizeof(bloque));
    log_info(fileSystemLogger, "Acceso a Bloque - Archivo: %s - Bloque Archivo: 2 - Bloque File System: %d", nombreArchivo, bloque);
    msync(bloques, sizeof(bloques), MS_SYNC);
}

void fcb_quitar_bloque(t_config* fcb, int cantBloquesEnPunteroIndirecto){
    uint32_t punteroIndirecto = config_get_int_value(fcb, "PUNTERO_INDIRECTO");
    int tamanioBitmap = (int) bitarray_get_max_bit(bitmapBitarray);
    int posicionBloqueIndirecto = punteroIndirecto * tamanioBloque; // Posicion en bloques, en bytes, del puntero indirecto, dentro de bloques.dat
    char* nombreArchivo = config_get_string_value(fcb, "NOMBRE_ARCHIVO");

    uint32_t bloqueAQuitar; // Variable para guardar el bloque que estamos sacando
    sleep(fileSystem_config_obtener_retardo_acceso_bloque(fileSystemConfig)/1000);
    memcpy(&bloqueAQuitar, bloques + posicionBloqueIndirecto * 64 + (cantBloquesEnPunteroIndirecto * 4 - 4 ), 4);    // Metemos en la variable auxiliar el bloque que vamos a sacar
    log_info(fileSystemLogger, "Acceso a Bloque - Archivo: %s - Bloque de Archivo: Puntero Indirecto - Bloque de FS: %d", nombreArchivo, bloqueAQuitar);                

    bitarray_clean_bit(bitmapBitarray, bloqueAQuitar);  // Actualizamos bitmap
    log_info(fileSystemLogger, "Acceso a Bitmap - Bloque: %d - Estado: 1 a 0", bloqueAQuitar);
    msync(bitmap, tamanioBitmap, MS_SYNC);
}

char* leerBloqueDirecto(uint32_t punteroDirecto, uint32_t cantBytes, uint32_t puntero, char* nombreArchivo){
    char* datos = malloc(cantBytes);
    sleep(fileSystem_config_obtener_retardo_acceso_bloque(fileSystemConfig)/1000);    
    memcpy(datos, bloques + punteroDirecto * tamanioBloque + puntero, cantBytes);
    log_info(fileSystemLogger, "Acceso a Bloque - Archivo: %s - Bloque de Archivo: 1 - Bloque de FS: %d", nombreArchivo, punteroDirecto);
    return datos;
}

char* leerBloques(t_config* fcb, uint32_t cantBytes, uint32_t puntero, char* nombreArchivo){
        uint32_t punteroDirecto = config_get_int_value(fcb, "PUNTERO_DIRECTO");
        uint32_t punteroIndirecto = config_get_int_value(fcb, "PUNTERO_INDIRECTO");
        
        char* datos = malloc(cantBytes);
        uint32_t bloqueDelPunteroDelArchivo = my_ceil((double) puntero / 64);   // Bloque donde está parado el puntero del proceso en el archivo
        if(bloqueDelPunteroDelArchivo == 0){
            bloqueDelPunteroDelArchivo = 1;
        }
        uint32_t posicionBloquePuntero = puntero % tamanioBloque;    // Byte del bloque del puntero, donde estás parado en el bloque
        uint32_t bloquesAAcceder = my_ceil((double) (posicionBloquePuntero + cantBytes) / 64);
        uint32_t restoALeerDeBloqueDelPuntero;
        if(puntero != 0){
            restoALeerDeBloqueDelPuntero = bloqueDelPunteroDelArchivo * 64 - puntero;
        } else {
            restoALeerDeBloqueDelPuntero = min(cantBytes, tamanioBloque);
        }

        uint32_t cantBytesRestantes = cantBytes;
        uint32_t bloqueSiguiente = bloqueDelPunteroDelArchivo + 1;

        char* aux = malloc(cantBytes);
        for(int i=0; i<bloquesAAcceder; i++){
            if(restoALeerDeBloqueDelPuntero != 0){
                if(bloqueDelPunteroDelArchivo == 1){
                    aux = leerBloqueDirecto(punteroDirecto, restoALeerDeBloqueDelPuntero, puntero, nombreArchivo);
                    datos = concat(datos, aux);
                    restoALeerDeBloqueDelPuntero = 0;   
                    cantBytesRestantes = cantBytesRestantes - restoALeerDeBloqueDelPuntero;
                }
                else{
                    uint32_t bloqueAAcceder;    
                    sleep(fileSystem_config_obtener_retardo_acceso_bloque(fileSystemConfig)/1000);    
                    memcpy(&bloqueAAcceder, bloques + punteroIndirecto * 64 + (bloqueDelPunteroDelArchivo - 2) * 4, 4);
                    log_info(fileSystemLogger, "Acceso a Bloque - Archivo: %s - Bloque de Archivo: Puntero Indirecto - Bloque de FS: %d", nombreArchivo, punteroIndirecto);
                    sleep(fileSystem_config_obtener_retardo_acceso_bloque(fileSystemConfig)/1000);    
                    memcpy(aux, bloques + bloqueAAcceder * tamanioBloque + posicionBloquePuntero, restoALeerDeBloqueDelPuntero);
                    log_info(fileSystemLogger, "Acceso a Bloque - Archivo: %s - Bloque de Archivo: %d - Bloque de FS: %d", nombreArchivo, bloqueDelPunteroDelArchivo, bloqueAAcceder);
                    datos = concat(datos, aux);
                    restoALeerDeBloqueDelPuntero = 0;   
                    cantBytesRestantes = cantBytesRestantes - restoALeerDeBloqueDelPuntero;
                }
            } else {
                // i = 1
                uint32_t bloqueAAcceder;   
                sleep(fileSystem_config_obtener_retardo_acceso_bloque(fileSystemConfig)/1000);    
                memcpy(&bloqueAAcceder, bloques+ punteroIndirecto * 64 + (bloqueSiguiente - 2) * 4, 4);
                log_info(fileSystemLogger, "Acceso a Bloque - Archivo: %s - Bloque de Archivo: Puntero Indirecto - Bloque de FS: %d", nombreArchivo, punteroIndirecto);

                if(cantBytesRestantes < 64){    // Ultima iteración básicamente, no tenes necesariamente que leer el último bloque ENTERO (64 bytes)
                    sleep(fileSystem_config_obtener_retardo_acceso_bloque(fileSystemConfig)/1000);    
                    memcpy(aux, bloques + bloqueAAcceder * tamanioBloque, cantBytesRestantes);
                    log_info(fileSystemLogger, "Acceso a Bloque - Archivo: %s - Bloque de Archivo: %d - Bloque de FS: %d", nombreArchivo,bloqueDelPunteroDelArchivo + i+2, bloqueAAcceder);                        
                    datos = concat(datos, aux);
                    free(aux);
                    return datos;
                }
                sleep(fileSystem_config_obtener_retardo_acceso_bloque(fileSystemConfig)/1000);    
                memcpy(aux, bloques + bloqueAAcceder * tamanioBloque, tamanioBloque);
                log_info(fileSystemLogger, "Acceso a Bloque - Archivo: %s - Bloque de Archivo: %d - Bloque de FS: %d", nombreArchivo, bloqueDelPunteroDelArchivo + i+2, bloqueAAcceder);            
                datos = concat(datos, aux);
                bloqueSiguiente = bloqueSiguiente + 1;
                cantBytesRestantes = cantBytesRestantes - tamanioBloque;
            }
        }
        return datos;
}

void escribirBloqueDirecto(uint32_t punteroDirecto, uint32_t cantBytes, uint32_t puntero, char* nombreArchivo, char* informacion){
    sleep(fileSystem_config_obtener_retardo_acceso_bloque(fileSystemConfig) / 1000);
    memcpy(bloques + punteroDirecto * 64 + puntero, informacion, cantBytes);
    log_info(fileSystemLogger, "Acceso a Bloque - Archivo: %s - Bloque de Archivo: 1 - Bloque de FS: %d", nombreArchivo, punteroDirecto);
}

t_config* encontrarFCB(char* nombreArchivoNuevo){

    bool encontrarArch(void* Aux){
        t_config* tab = (t_config*) Aux; 
                    return strcmp(config_get_string_value(tab,"NOMBRE_ARCHIVO"),nombreArchivoNuevo) == 0;
    }
    return list_find(listaFCBsAbiertos, encontrarArch); 

}

t_config* config_iniciar_fcb(char* pathAlConfig) {
    t_config* config = config_create(pathAlConfig);
    if (NULL == config) {
        log_error(fileSystemLogger, "el path \"%s\" no se encontro", pathAlConfig);
        exit(-1);
    }
    return config;
}


char* concat(const char* s1, const char* s2){
    char* result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void crearArchivoFCB(char* NombreArchivo){

    char* ruta = concat(PATH_FCB, NombreArchivo);    
    int fd = open(ruta, O_CREAT | O_RDWR, S_IRWXU);   

    char* Nom = concat("NOMBRE_ARCHIVO=", NombreArchivo);
    char* Tam = "\nTAMANIO_ARCHIVO=0";
    char* Pdi = "\nPUNTERO_DIRECTO=-1";
    char* Pin = "\nPUNTERO_INDIRECTO=-1";

    size_t tamanioMap = strlen(Nom) + strlen(Tam) + strlen(Pdi) + strlen(Pin);

    ftruncate(fd, tamanioMap);

    void* mapArchivo = mmap(NULL, tamanioMap, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 

    void* punteroAlFin = mempcpy(mapArchivo, Nom, strlen(Nom));
    punteroAlFin = mempcpy(punteroAlFin, Tam, strlen(Tam));
    punteroAlFin = mempcpy(punteroAlFin, Pdi, strlen(Pdi));
    punteroAlFin = mempcpy(punteroAlFin, Pin, strlen(Pin));

    msync(mapArchivo, tamanioMap, MS_SYNC);
    
    log_info(fileSystemLogger, "Crear Archivo: %s", NombreArchivo);

    munmap(mapArchivo,tamanioMap);
    free(ruta);
    free(Nom);
    close(fd);
}
