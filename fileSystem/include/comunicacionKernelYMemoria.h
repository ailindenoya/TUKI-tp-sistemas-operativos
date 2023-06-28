#include "fileSystem_config.h"
#include "bitmap.h"
#include <stdlib.h>
#include <stdio.h>
#include "../../utils/include/stream.h"
#include <fcntl.h>
#include "../include/fcb.h"
#include "../../utils/include/flagsParaCommons.h"
#include <string.h>
#include<math.h>


void F_OPEN(char*);
void F_TRUNCATE(char*, uint32_t);
void atenderPeticionesDeKernel(void);
void dispatch_FS_peticiones_de_Kernel(void);