#include "../include/fcb.h"
#define PATH_FCB "../fcb/"


extern t_log* fileSystemLogger;

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
    self->PUNTERO_DIRECTO = 0;
    self->PUNTERO_INDIRECTO = 0;

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
    int fd = open(NombreArchivo, O_CREAT | O_RDWR, S_IRWXU);   

    char* Nom = concat("NOMBRE_ARCHIVO=", NombreArchivo);
    char* Tam = "\nTAMANIO_ARCHIVO=0";
    char* Pdi = "\nPUNTERO_DIRECTO=0";
    char* Pin = "\nPUNTERO_INDIRECTO=0";

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