#ifndef CONSOLA_PARSER_H_INCLUDED
#define CONSOLA_PARSER_H_INCLUDED

#include <commons/string.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>

#include "../include/consola_serializador.h"
#include "../../utils/include/flagsParaCommons.h"
#include "../../utils/include/buffer.h"

bool consola_parsear_instrucciones(t_buffer *buffer, const char *pathInstrucciones, t_log *consolaLogger);
#endif