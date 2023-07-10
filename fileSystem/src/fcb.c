#define _GNU_SOURCE
#include "../include/fcb.h"
#include "../include/bitmap.h"
#define PATH_FCB "fcb/"
#define INTMAX_MAX_P1 ((INTMAX_MAX/2 + 1)*2.0)

extern t_log* fileSystemLogger;
extern t_list* listaFCBsAbiertos;
extern t_superbloque_config* superbloqueConfig;
extern t_fileSystem_config* fileSystemConfig;

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

void fcb_asignar_bloque(t_config* fcb, uint32_t bloque, uint32_t bloquesAsignadosEnPunteroIndirecto){
    if (config_get_int_value(fcb,"PUNTERO_DIRECTO") == -1){  
        char* bloq = string_itoa(bloque);
        config_set_value(fcb, "PUNTERO_DIRECTO",bloq);
        return;
    }
    
    uint32_t tamanioBloque = superbloque_config_obtener_block_size(superbloqueConfig);
    uint32_t punteroIndirecto = config_get_int_value(fcb,"PUNTERO_INDIRECTO");

    if (punteroIndirecto == -1){   //HAY QUE ASIGNAR OTRO BLOQUE 
        char* bloq = string_itoa(bloque);   
        config_set_value(fcb, "PUNTERO_INDIRECTO",bloq);
        uint32_t blqueNuevoAAsignar = buscarBloqueLibre();
        fcb_asignar_bloque(fcb,blqueNuevoAAsignar, bloquesAsignadosEnPunteroIndirecto);
        return;
    }
    int posicionBloqueIndirecto = punteroIndirecto * tamanioBloque;
    char* nombreArchivo = config_get_string_value(fcb, "NOMBRE_ARCHIVO");
    log_info(fileSystemLogger, "Acceso a Bloque - Archivo: %s - Bloque Archivo: 2 - Bloque File System: %d", nombreArchivo, bloque);

    sleep(fileSystem_config_obtener_retardo_acceso_bloque(fileSystemConfig)/1000);
    memcpy(bloques + posicionBloqueIndirecto + bloquesAsignadosEnPunteroIndirecto * 4, &bloque, sizeof(bloque));
    msync(bloques, superbloque_config_obtener_block_size(superbloqueConfig), MS_SYNC);
}

void fcb_quitar_bloque(t_config* fcb, int cantBloquesEnPunteroIndirecto){
    uint32_t punteroIndirecto = config_get_int_value(fcb, "PUNTERO_INDIRECTO");
    char* nombreArchivo = config_get_string_value(fcb, "NOMBRE_ARCHIVO");

    log_info(fileSystemLogger, "Puntero Indirecto: %d", punteroIndirecto);

    int posicionBloqueIndirecto = punteroIndirecto * superbloque_config_obtener_block_size(superbloqueConfig);
    sleep(fileSystem_config_obtener_retardo_acceso_bloque(fileSystemConfig)/1000);
    uint32_t aux = -1;
    memcpy(bloques + posicionBloqueIndirecto + (cantBloquesEnPunteroIndirecto - 4 ) * 4, &aux, sizeof(aux));

    log_info(fileSystemLogger, "Acceso a Bloque - Archivo: %s - Bloque Archivo: 2 - Bloque File System: %d", nombreArchivo, punteroIndirecto);
    msync(bloques, 4, MS_SYNC);
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
