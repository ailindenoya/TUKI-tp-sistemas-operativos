#include "../include/bitmap.h"

extern t_log* fileSystemLogger;
extern t_superbloque_config* superbloqueConfig;

t_bitarray* cargarBitMap(){
    int bytes = superbloque_config_obtener_block_count(superbloqueConfig) / 8;  // Dividis cantidad de bloques por 8 para obtener los bytes
    t_bitarray* bitArray;

    int fd = open("../bitmap.dat", O_RDWR, S_IRUSR | S_IWUSR);
    struct stat sb;
    if (fstat(fd, &sb) == -1){
        log_info(fileSystemLogger, "No se pudo obtener los datos del archivo bitmap");
    }

    void* bitmap = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    bitArray = bitarray_create(bitmap, bytes);
    
    return bitArray;
}