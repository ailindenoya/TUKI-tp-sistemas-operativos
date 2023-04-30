#include <commons/collections/list.h>
#include <stdint.h>
#include <cpu_config.h>
#include <commons/log.h>

struct t_contexto {
    uint32_t pid;
    uint32_t programCounter;
    t_list* instrucciones;
};

t_contexto* crear_contexto(uint32_t pid, uint32_t programCounter);
void contexto_destruir(t_contexto* self);
