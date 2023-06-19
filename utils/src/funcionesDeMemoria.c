
#include "../include/funcionesDeMemoria.h"
#include <stdint.h>
#include <string.h>


struct segmento{
    int id;
    int base; 
    int tamanio; 
};




segmento* crear_segmento(int id, int base, int tamanio){
    segmento* seg =  malloc(sizeof(*seg));
    seg->id = id;
    seg->base = base;
    seg->tamanio = tamanio;
}


