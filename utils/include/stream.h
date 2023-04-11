#ifndef STREAM_H_INCLUDED
#define STREAM_H_INCLUDED

#include "buffer.h"

static void stream_enviar(int toSocket, void* streamToSend, uint32_t bufferSize);
static void* stream_crear(uint8_t header, t_buffer* buffer);
void stream_recibir_buffer(int origenSocket, t_buffer* destBuffer);
void stream_enviar_buffer(int destinoSocket, uint8_t header, t_buffer* buffer);
void stream_enviar_buffer_vacio(int destinoSocket, uint8_t header);
void stream_recibir_buffer_vacio(int origenSocket);
uint8_t stream_recibir_header(int origenSocket);

#endif