#include "../include/bitmap.h"
#include <errno.h>
 
extern int errno;
extern t_log* fileSystemLogger;
extern t_superbloque_config* superbloqueConfig;

t_bitarray* bitArray;

void limpiarPosiciones(t_bitarray* unEspacio, int posicionInicial, int tamanioProceso) {
	int i = 0;
	for (i = posicionInicial; i < posicionInicial + tamanioProceso; i++) {
		bitarray_clean_bit(unEspacio, i);
	}
}

t_bitarray* cargarBitMap(){
    int bytes = superbloque_config_obtener_block_count(superbloqueConfig) / 8;  // Dividis cantidad de bloques por 8 para obtener los bytes
    t_bitarray* bitArray;

    int fd = open("bitmap.dat", O_CREAT | O_RDWR, S_IRWXU); // SI NO EXISTE EL ARCHIVO LO CREA, CAPAZ PODEMOS CAMBIAR LA RUTA

    if (fd == -1){
        log_info(fileSystemLogger, "No se pudo abrir el archivo Bitmap");
    }

    ftruncate(fd, bytes);  // SI EL ARCHIVO ES DE MENOS TAMAÑO QUE "bytes" ENTONCES LO EXTIENDE LLENANDOLO CON '\n'

    struct stat sb;
    if (fstat(fd, &sb) == -1){
        log_info(fileSystemLogger, "No se pudo obtener los datos del archivo bitmap");
    }

    void* bitmap = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if(bitmap == MAP_FAILED){
        log_error(fileSystemLogger, "Error al usar mmap");
    }

    bitArray = bitarray_create_with_mode((char*) bitmap, bytes , LSB_FIRST);
    
    //marco libres todos las posiciones del array
    limpiarPosiciones(bitArray, 0, bytes);  // ESTO HABRIA QUE HACERLO SOLO CUANDO LO INICIAMOS POR PRIMERA VEZ
    
    for(int x =0;x<8000;x++){  // ESTO LO HICE PARA VER QUE HAY EN EL BITARRAY
        printf("%d", bitarray_test_bit(bitArray, x));
    }
    
    int sincronizacion = msync(bitmap, bytes, MS_SYNC);
    if (sincronizacion == -1){
        log_info(fileSystemLogger, "Error al sincronizar el mmap de bitmap con disco");
        perror("msync");
    }

    int finMmap = munmap(bitmap, bytes);
    if (finMmap == -1){
        log_info(fileSystemLogger, "Error al unmapear el bitmap de memoria");
        perror("munmap");
    }
    
    close(fd);

    printf("\nSE CERRO\n"); // esto lo hice para ver si llegaba a cerrar el archivo y hacer el munmap

    return bitArray;
}