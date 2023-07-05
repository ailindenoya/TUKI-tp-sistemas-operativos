#ifndef FUNCIONESDEREGISTROSDECPU_H_INCLUDED
#define FUNCIONESDEREGISTROSDECPU_H_INCLUDED

#include "buffer.h"

typedef struct registros {
    char AX[4];
    char BX[4];
    char CX[4];
    char DX[4];
    char EAX[8];
    char EBX[8];
    char ECX[8];
    char EDX[8];
    char RAX[16];
    char RBX[16];
    char RCX[16];
    char RDX[16];
}registros; 



void buffer_empaquetar_registros(t_buffer* buffer, registros* reg);
void buffer_desempaquetar_registros(t_buffer* buffer, registros* reg);
void registros_imprimir(registros*);


#endif