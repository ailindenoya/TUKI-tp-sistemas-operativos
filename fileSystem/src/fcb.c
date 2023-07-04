#define _GNU_SOURCE
#include "../include/fcb.h"
#include "../include/bitmap.h"
#define PATH_FCB "fcb/"


extern t_log* fileSystemLogger;
extern t_list* listaFCBsAbiertos;
extern t_superbloque_config* superbloqueConfig;

extern int fdBitmap;
extern int fdBloques;
extern void* bitmap;
extern void* bloques;

void fcb_asignar_bloque(t_config* fcb, uint32_t bloque){
    if (config_get_int_value(fcb,"PUNTERO_DIRECTO") == -1){  /// 
        char* bloq = string_itoa(bloque);
        config_set_value(fcb, "PUNTERO_DIRECTO",bloq);
        return;
    }
    
    uint32_t punteroIndirecto = config_get_int_value(fcb,"PUNTERO_INDIRECTO");

    if (punteroIndirecto == -1){   //HAY QUE ASIGNAR OTRO BLOQUE 
        char* bloq = string_itoa(bloque);   
        config_set_value(fcb, "PUNTERO_INDIRECTO",bloq);
        uint32_t blqueNuevoAAsignar = buscarBloqueLibre();
        fcb_asignar_bloque(fcb,blqueNuevoAAsignar);
        return;
    }
    int posicionBloqueIndirecto = punteroIndirecto * superbloque_config_obtener_block_size(superbloqueConfig);

    int bloquesAsignadosEnPunteroIndirecto = (int) ceil((config_get_int_value(fcb,"TAMANIO_ARCHIVO") - 64) / 64);
    log_info(fileSystemLogger, "fcb obtener tam arch = %d", config_get_int_value(fcb,"TAMANIO_ARCHIVO"));
    memcpy(bloques + posicionBloqueIndirecto + bloquesAsignadosEnPunteroIndirecto * 4, &bloque, sizeof(bloque));  // Segmentation fault acá

/*
    void* aux = malloc(superbloque_config_obtener_block_size(superbloqueConfig));
    void* punteroAlFin = mempcpy(aux, bloques, sizeof(uint32_t));  // Copias el primer puntero dentro del bloque de punteros
    while ((uint32_t*) aux != '/0'){
        punteroAlFin = mempcpy(aux, bloques, sizeof(uint32_t))
    }
    memcpy(aux, bloque, sizeof(bloque));   
    memcpy(bloques, aux, sizeof(aux));     
    */
    msync(bloques, superbloque_config_obtener_block_size(superbloqueConfig), MS_SYNC);
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
