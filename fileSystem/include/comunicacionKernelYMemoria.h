#include "fileSystem_config.h"
#include <stdlib.h>
#include "../../utils/include/stream.h"
#include <fcntl.h>
#include "../include/fcb.h"
#include "../../utils/include/flagsParaCommons.h"
#include <string.h>
#include<math.h>


void F_OPEN(char*);
void F_TRUNCATE(char*, uint32_t);
void atenderPeticionesDeKernel();