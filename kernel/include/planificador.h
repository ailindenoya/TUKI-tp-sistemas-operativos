#include "../include/kernel_config.h"
#include "../include/pcb.h"
#include <pthread.h>


typedef t_pcb* t_dispatch_handler_fn(t_estado*, double);


