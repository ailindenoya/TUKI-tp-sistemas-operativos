#include "../utils/include/buffer.h"

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

