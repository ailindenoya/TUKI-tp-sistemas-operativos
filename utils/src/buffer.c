#include "../include/buffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



t_buffer* buffer_crear(void) {
    t_buffer* self = malloc(sizeof(*self));
    self->size = 0;
    self->stream = NULL;
    return self;
}

void buffer_destruir(t_buffer* self) {
    free(self->stream);
    free(self);
}

void buffer_empaquetar(t_buffer* self, void* streamToAdd, int size) {
    self->stream = realloc(self->stream, self->size + size);
    memcpy(self->stream + self->size, streamToAdd, size);
    self->size += size;
}

void buffer_desempaquetar(t_buffer* self, void* dest, int size) {
    if (self->stream == NULL || self->size == 0) {
        puts("\e[0;31mbuffer_unpack: Error en el desempaquetado del buffer\e[0m");
        exit(-1);
    }
    memcpy(dest, self->stream, size);
    self->size -= size;
    memmove(self->stream, self->stream + size, self->size);
    self->stream = realloc(self->stream, self->size);
}

