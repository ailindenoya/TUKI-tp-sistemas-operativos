#ifndef FUNCIONES_DE_MEMORIA_H_INCLUDED
#define FUNCIONES_DE_MEMORIA_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>


typedef struct segmento segmento; 


segmento* crear_segmento(int id, int base, int tamanio);


#endif