#ifndef CONSOLA_SERIALIZADOR_H_INCLUDED
#define CONSOLA_SERIALIZADOR_H_INCLUDED

#include "../../utils/include/buffer.h"


void consola_serializador_empaquetar_sin_parametros(t_buffer* buffer, uint8_t instruccion);
void consola_serializador_empaquetar_un_parametro(t_buffer* buffer, uint8_t instruccion, uint32_t parametro1);
void consola_serializador_empaquetar_dos_parametros(t_buffer* buffer, uint8_t instruccion, uint32_t parametro1,uint32_t parametro2);
void consola_serializador_empaquetar_tres_parametros(t_buffer* buffer, uint8_t instruccion,uint32_t parametro1,uint32_t parametro2, uint32_t parametro3);

#endif
