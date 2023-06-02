#include "../include/bitmap.h"

extern t_log* fileSystemLogger;
extern t_superbloque_config* superbloqueConfig;


t_bitarray* cargarBitMap(){
    t_bitarray* bitArray;
    int bytes = superbloque_config_obtener_block_count(superbloqueConfig) / 8;
    
    int fd = open("../bitmap.dat", O_RDONLY, S_IRUSR);
    struct stat sb;
    if (fstat(fd, &sb) == -1){
        log_info(fileSystemLogger, "No se pudo obtener los datos del archivo bitmap");
    }

    char* bits = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);

    if (!existenDatos()){
        bitArray = bitarray_create(malloc(bytes), bytes);
        return bitArray;
    }

    bitArray = bitarray_create(bits, bytes);
    return bitArray;
}

bool existenDatos(){
    // verificar si el archivo bitmap.dat tiene datos o no, si está vacío o no
    return true;
}