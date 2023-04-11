#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#include <stdint.h>

typedef struct {
    uint32_t size;
    void *stream;
} t_buffer;

t_buffer* buffer_crear(void);
void buffer_destruir(t_buffer* self);

#endif 