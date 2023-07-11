#include "../include/bitmap.h"
#include <errno.h>
 
extern uint32_t tamanioBloque;
extern int errno;
extern t_log* fileSystemLogger;
extern t_superbloque_config* superbloqueConfig;
extern t_fileSystem_config* fileSystemConfig;

extern t_bitarray* bitmapBitarray;
int fdBitmap;
int fdBloques;
void* bitmap;
void* bloques;

void agregarBloques(int cantidadBloques, t_config* fcb, uint32_t bloquesAsignadosEnPunteroIndirecto){

    int tamanioBitmap = (int) bitarray_get_max_bit(bitmapBitarray);
    int aux = 0;
    log_info(fileSystemLogger, "tamanio bitmap: %d", tamanioBitmap);
    for (int i = 0; i < tamanioBitmap; i++){
        
        if (bitarray_test_bit(bitmapBitarray, i) == false){
            bitarray_set_bit(bitmapBitarray, i);    // Modificación de Bitmap
            log_info(fileSystemLogger, "Acceso a Bitmap - Bloque: %d - Estado: 0 a 1", i);
            msync(bitmap, tamanioBitmap, MS_SYNC);
            fcb_asignar_bloque(fcb, i, bloquesAsignadosEnPunteroIndirecto); // Modificación de Bloque
            aux++;
        }
        if (aux == cantidadBloques){
            return;
        }
    }
}

void quitarBloques(int cantidadBloques, t_config* fcb, uint32_t tamanioViejo){

    int bloquesAsignadosEnPunteroIndirecto = (int) my_ceil((double) (tamanioViejo - tamanioBloque) / tamanioBloque);
    log_info(fileSystemLogger, "Cantidad de bloques en puntero indirecto: %d", bloquesAsignadosEnPunteroIndirecto);
    
    for(int i=0;i<cantidadBloques;i++){
        uint32_t bloqueDelArchivo = i + 2;  // El 1 es el Directo, que solo se quita en caso de F_TRUNCATE(nombreArchivo, 0)
        fcb_quitar_bloque(fcb, bloquesAsignadosEnPunteroIndirecto, bloqueDelArchivo);
        if(bloquesAsignadosEnPunteroIndirecto == 1){
            config_set_value(fcb, "PUNTERO_INDIRECTO", "-1");
        }
        bloquesAsignadosEnPunteroIndirecto = bloquesAsignadosEnPunteroIndirecto - 1;
    }
}

uint32_t buscarBloqueLibre(){
    int tamanioBitmap = (int) bitarray_get_max_bit(bitmapBitarray);
    for (int i = 0; i < tamanioBitmap; i++){
        if (bitarray_test_bit(bitmapBitarray, i)){
            bitarray_set_bit(bitmapBitarray, i);
            return i;
        }
    }
    return -1;
}


void limpiarPosiciones(t_bitarray* unEspacio, int posicionInicial, int tamanioProceso) {
	int i = 0;
	for (i = posicionInicial; i < posicionInicial + tamanioProceso; i++) {
		bitarray_clean_bit(unEspacio, i);
	}
}

void cargarBitMap(){
    int bytes = superbloque_config_obtener_block_count(superbloqueConfig) / 8;  // Dividis cantidad de bloques por 8 para obtener los bytes
    bool existeBitmap = true;   // Para chequear si el bitmap existe de una ejecución previa del sistema

    fdBitmap = open("bitmap.dat", O_CREAT | O_RDWR, S_IRWXU); // SI NO EXISTE EL ARCHIVO LO CREA, CAPAZ PODEMOS CAMBIAR LA RUTA

    if (fdBitmap == -1){
        log_info(fileSystemLogger, "No se pudo abrir el archivo Bitmap");
    }

    ftruncate(fdBitmap, bytes);  // SI EL ARCHIVO ES DE MENOS TAMAÑO QUE "bytes" ENTONCES LO EXTIENDE LLENANDOLO CON '\0'

    bitmap = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fdBitmap, 0);

    if(bitmap == MAP_FAILED){
        log_error(fileSystemLogger, "Error al usar mmap");
    }

    bitmapBitarray = bitarray_create_with_mode((char*) bitmap, bytes , LSB_FIRST);
    
    //marco libres todos las posiciones del array
    if (existeBitmap == false){
        limpiarPosiciones(bitmapBitarray, 0, bytes);  // Si es la primera ejecución del sistema, se carga el bitmap con ceros, todos bloques libres
    }
    
    int contador =0;
    // Descomentar esto de abajo si se quiere checkear los valores del bitarray en pantalla
    for(int x =0;x<8000;x++){  // ESTO LO HICE PARA VER QUE HAY EN EL BITARRAY
         bitarray_test_bit(bitmapBitarray, x);
         contador++;
    }
    log_info(fileSystemLogger, "contador: %d", contador);
    
    int sincronizacion = msync(bitmap, bytes, MS_SYNC);
    if (sincronizacion == -1){
        log_info(fileSystemLogger, "Error al sincronizar el mmap de bitmap con disco");
        perror("msync");
    }
    /*
    int finMmap = munmap(bitmap, bytes);
    if (finMmap == -1){
        log_info(fileSystemLogger, "Error al unmapear el bitmap de memoria");
        perror("munmap");
    }

    close(fdBitmap);
    */
    printf("\nSE CERRO\n"); // esto lo hice para ver si llegaba a cerrar el archivo y hacer el munmap
}

void cargarArchivoDeBloques(){
    fdBloques = open("bloques.dat", O_CREAT | O_RDWR, S_IRWXU);
    uint32_t tamanioArchivoBloques = superbloque_config_obtener_block_count(superbloqueConfig) * superbloque_config_obtener_block_size(superbloqueConfig);
    ftruncate(fdBloques, tamanioArchivoBloques);
    bloques = mmap(NULL, tamanioArchivoBloques, PROT_READ | PROT_WRITE, MAP_SHARED, fdBloques, 0);
}