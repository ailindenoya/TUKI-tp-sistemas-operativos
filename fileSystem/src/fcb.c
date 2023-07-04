#define _GNU_SOURCE
#include "../include/fcb.h"
#define PATH_FCB "fcb/"


extern t_log* fileSystemLogger;
extern t_list* listaFCBsAbiertos;
extern t_superbloque_config* superbloqueConfig;


struct t_fcb {
    char* NOMBRE_ARCHIVO;
    uint32_t TAMANIO_ARCHIVO;
    uint32_t PUNTERO_DIRECTO;   // Primer bloque donde están los datos del archivo
    uint32_t PUNTERO_INDIRECTO; // Puntero a los siguientes demás bloques donde hay datos del archivo
};

t_fcb* fcb_crear(char* Nombre){
    t_fcb* self = malloc(sizeof(*self));

    self->NOMBRE_ARCHIVO = Nombre;
    self->TAMANIO_ARCHIVO = 0;
    self->PUNTERO_DIRECTO = -1;
    self->PUNTERO_INDIRECTO = -1;

    return self;
}

void fcb_asignar_bloque(t_fcb* fcb, uint32_t bloque){
    if (fcb_obtener_puntero_directo(fcb) == -1){  /// 
        fcb_setear_puntero_directo(fcb, bloque);
        return;
    }

    uint32_t punteroIndirecto = fcb_obtener_puntero_indirecto(fcb);


    if (punteroIndirecto == -1){
        fcb_setear_puntero_indirecto(fcb, bloque);
        return;
    }

    int fd = open("bloques.dat", O_RDWR, S_IRWXU);


    if (fd == -1){
        log_info(fileSystemLogger, "No se pudo abrir el archivo de Bloques");
    }

    off_t offset = punteroIndirecto * superbloque_config_obtener_block_size(superbloqueConfig);

    void* bloques = mmap(NULL, superbloque_config_obtener_block_size(superbloqueConfig), PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);

    int bloquesAsignadosEnPunteroIndirecto = (int) ceil((fcb_obtener_tamanio_archivo(fcb) - 64) / 64);
    log_info(fileSystemLogger, "fcb obtener tam arch = %d", fcb_obtener_tamanio_archivo(fcb));
    memcpy(bloques + bloquesAsignadosEnPunteroIndirecto * 4, &bloque, sizeof(bloque));  // Segmentation fault acá

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
    munmap(bloques, superbloque_config_obtener_block_size(superbloqueConfig));
    close(fd);
}

t_fcb* encontrarFCB(char* nombreArchivoNuevo){

    bool encontrarArch(void* Aux){
        t_fcb* tab = (t_fcb*) Aux; 
                    return strcmp(fcb_obtener_nombre_archivo(tab),nombreArchivoNuevo) == 0;
    }
    return list_find(listaFCBsAbiertos, encontrarArch); 

}

int config_iniciar_fcb(void* config, char* pathAlConfig, t_log* logger,void (*config_initializer)(void* moduleConfig, t_config* configTemp)) {
    t_config* configTemp = config_create(pathAlConfig);
    if (NULL == configTemp) {
        log_error(logger, "el path \"%s\" no se encontro", pathAlConfig);
        return -1;
    }
    config_initializer(config, configTemp);
    config_destroy(configTemp);
    return 1;
}

void fcb_config_iniciar(void* moduleConfig, t_config* tempCfg){
    t_fcb* fcbConfig = (t_fcb*) moduleConfig;

    fcbConfig->NOMBRE_ARCHIVO = strdup(config_get_string_value(tempCfg, "NOMBRE_ARCHIVO"));
    fcbConfig->TAMANIO_ARCHIVO = config_get_int_value(tempCfg, "TAMANIO_ARCHIVO");
    fcbConfig->PUNTERO_DIRECTO = config_get_int_value(tempCfg, "PUNTERO_DIRECTO");
    fcbConfig->PUNTERO_INDIRECTO = config_get_int_value(tempCfg, "PUNTERO_INDIRECTO");
   
}

t_fcb* fcb_config_crear(char* path, t_log* fileSystemLogger){
    t_fcb* self = malloc(sizeof(*self));
    config_iniciar_fcb(self, path, fileSystemLogger, fcb_config_iniciar);
    return self;
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
    munmap(mapArchivo, tamanioMap);
    close(fd);
    
    log_info(fileSystemLogger, "Crear Archivo: %s", NombreArchivo);

    free(ruta);
    free(Nom);
}

char* fcb_obtener_nombre_archivo(t_fcb* self){
    return self->NOMBRE_ARCHIVO;
}

uint32_t fcb_obtener_tamanio_archivo(t_fcb* self){
    return self->TAMANIO_ARCHIVO;
}

uint32_t fcb_obtener_puntero_directo(t_fcb* self){
    return self->PUNTERO_DIRECTO;
}

uint32_t fcb_obtener_puntero_indirecto(t_fcb* self){
    return self->PUNTERO_INDIRECTO;
}

void fcb_setear_tamanio_archivo(t_fcb* self, uint32_t tamanio){
    self->TAMANIO_ARCHIVO = tamanio;
}

void fcb_setear_puntero_directo(t_fcb* self, uint32_t punteroDirecto){
    self->PUNTERO_DIRECTO = punteroDirecto;
}

void fcb_setear_puntero_indirecto(t_fcb* self, uint32_t punteroIndirecto){
    self->PUNTERO_INDIRECTO = punteroIndirecto;
}
