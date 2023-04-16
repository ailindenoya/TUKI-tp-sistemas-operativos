#include "../include/stream.h"
#include "../include/buffer.h"


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>


void stream_enviar(int toSocket, void* streamToSend, uint32_t bufferSize) {
    uint8_t header = 0;
    uint32_t tamanio = 0;
    ssize_t bytesEnviados = send(toSocket, streamToSend, sizeof(header) + sizeof(tamanio) + bufferSize, 0);
    if (bytesEnviados == -1) {
        printf("error al enviar stream [%s]", strerror(errno));
    }
}

void* stream_crear(uint8_t header, t_buffer* buffer) {
    void* streamAEnviar = malloc(sizeof(header) + sizeof(buffer->size) + buffer->size);
    int offset = 0;
    memcpy(streamAEnviar + offset, &header, sizeof(header));
    offset += sizeof(header);
    memcpy(streamAEnviar + offset, &(buffer->size), sizeof(buffer->size));
    offset += sizeof(buffer->size);
    memcpy(streamAEnviar + offset, buffer->stream, buffer->size);
    return streamAEnviar;
}


void stream_recibir_buffer(int origenSocket, t_buffer* destBuffer) {
    ssize_t bytesDeMensaje = recv(origenSocket, &(destBuffer->size), sizeof(destBuffer->size), 0);
    if (bytesDeMensaje == -1) {
        printf("error al recibir buffer [%s]", strerror(errno));
    } else if (destBuffer->size > 0) {
        destBuffer->stream = malloc(destBuffer->size);
        recv(origenSocket, destBuffer->stream, destBuffer->size, 0);
    }
}


void stream_enviar_buffer(int destinoSocket, uint8_t header, t_buffer* buffer) {
    void* stream = stream_crear(header, buffer);
    stream_enviar(destinoSocket, stream, buffer->size);
    free(stream);
}


void stream_enviar_buffer_vacio(int destinoSocket, uint8_t header) {
    t_buffer* emptyBuffer = buffer_crear();
    stream_enviar_buffer(destinoSocket, header, emptyBuffer);
    buffer_destruir(emptyBuffer);
}



void stream_recibir_buffer_vacio(int origenSocket) {
    t_buffer* buffer = buffer_crear();
    stream_recibir_buffer(origenSocket, buffer);
    buffer_destruir(buffer);
}



uint8_t stream_recibir_header(int origenSocket) {
    uint8_t header;
    ssize_t bytesDeMensaje = recv(origenSocket, &header, sizeof(header), 0);
    if (bytesDeMensaje == -1) {
        printf("error al recibir header [%s]", strerror(errno));
    }
    return header;
}
