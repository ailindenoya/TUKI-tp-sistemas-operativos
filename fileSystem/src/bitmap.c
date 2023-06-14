#include "../include/bitmap.h"
#include <errno.h>
 
extern int errno;
extern t_log* fileSystemLogger;
extern t_superbloque_config* superbloqueConfig;

t_bitarray* bitArray;

t_bitarray* cargarBitMap(){
    int bytes = superbloque_config_obtener_block_count(superbloqueConfig) / 8;  // Dividis cantidad de bloques por 8 para obtener los bytes
    t_bitarray* bitArray;

    int fd = open("/home/utnso/tp-2023-1c-Cinco-Hispanos/fileSystem/bitmap.dat", O_RDWR);

    if (fd == -1){
        log_info(fileSystemLogger, "No se pudo abrir el archivo Bitmap");
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1){
        log_info(fileSystemLogger, "No se pudo obtener los datos del archivo bitmap");
    }

    void* bitmap = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    bitArray = bitarray_create_with_mode(bitmap, bytes, LSB_FIRST);

    //printf ("%p", &bitmap);
    //printf ("%d\n", bitarray_test_bit(bitArray, 1));
    //memset(bitArray->bitarray, 0, 1);
    bitarray_set_bit(bitArray, 0);
    


    // int sincronizacion = msync(bitmap, sb.st_size, MS_SYNC);
    // if (sincronizacion == -1){
    //     log_info(fileSystemLogger, "Error al sincronizar el mmap de bitmap con disco");
    //     perror("msync");
    // }

    int finMmap = munmap(bitmap, bytes);
    if (finMmap == -1){
        log_info(fileSystemLogger, "Error al unmapear el bitmap de memoria");
        perror("munmap");
    }

    close(fd);

    return bitArray;
}