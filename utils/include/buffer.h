#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#include <stdint.h>

typedef struct {
    uint32_t size;
    void *stream;
} t_buffer;

t_buffer* buffer_crear(void);
void buffer_destruir(t_buffer* self);
void buffer_empaquetar(t_buffer* self, void* streamToAdd, int size);
void buffer_empaquetar_string(t_buffer* self, char* string);
void buffer_desempaquetar(t_buffer* self, void* dest, int size);
void buffer_desempaquetar_string(t_buffer* self, char** pString);
#endif 