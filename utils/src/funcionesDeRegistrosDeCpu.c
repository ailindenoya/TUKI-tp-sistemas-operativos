#include "../include/funcionesDeRegistrosDeCpu.h"
#include <string.h>
#include <stdio.h>

#define TAM_REGISTRO_GENERAL 4
#define TAM_REGISTRO_EXTENDIDO 8
#define TAM_REGISTRO_R 16

void buffer_empaquetar_registros(t_buffer* buffer, registros* regs){
    buffer_empaquetar(buffer, regs->AX, sizeof(*(regs->AX)) * TAM_REGISTRO_GENERAL);
    buffer_empaquetar(buffer, regs->BX, sizeof(*(regs->BX)) * TAM_REGISTRO_GENERAL);
    buffer_empaquetar(buffer, regs->CX, sizeof(*(regs->CX)) * TAM_REGISTRO_GENERAL);
    buffer_empaquetar(buffer, regs->DX, sizeof(*(regs->DX)) * TAM_REGISTRO_GENERAL);
    buffer_empaquetar(buffer, regs->EAX, sizeof(*(regs->EAX)) * TAM_REGISTRO_EXTENDIDO);
    buffer_empaquetar(buffer, regs->EBX, sizeof(*(regs->EBX)) * TAM_REGISTRO_EXTENDIDO);
    buffer_empaquetar(buffer, regs->ECX, sizeof(*(regs->ECX)) * TAM_REGISTRO_EXTENDIDO);
    buffer_empaquetar(buffer, regs->EDX, sizeof(*(regs->EDX)) * TAM_REGISTRO_EXTENDIDO);
    buffer_empaquetar(buffer, regs->RAX, sizeof(*(regs->RAX)) * TAM_REGISTRO_R);
    buffer_empaquetar(buffer, regs->RBX, sizeof(*(regs->RBX)) * TAM_REGISTRO_R);
    buffer_empaquetar(buffer, regs->RCX, sizeof(*(regs->RCX)) * TAM_REGISTRO_R);
    buffer_empaquetar(buffer, regs->RDX, sizeof(*(regs->RDX)) * TAM_REGISTRO_R);
}

void buffer_desempaquetar_registros(t_buffer* buffer, registros* regs){
    buffer_desempaquetar(buffer, regs->AX, sizeof(*(regs->AX)) * TAM_REGISTRO_GENERAL);
    buffer_desempaquetar(buffer, regs->BX, sizeof(*(regs->BX)) * TAM_REGISTRO_GENERAL);
    buffer_desempaquetar(buffer, regs->CX, sizeof(*(regs->CX)) * TAM_REGISTRO_GENERAL);
    buffer_desempaquetar(buffer, regs->DX, sizeof(*(regs->DX)) * TAM_REGISTRO_GENERAL);
    buffer_desempaquetar(buffer, regs->EAX, sizeof(*(regs->EAX)) * TAM_REGISTRO_EXTENDIDO);
    buffer_desempaquetar(buffer, regs->EBX, sizeof(*(regs->EBX)) * TAM_REGISTRO_EXTENDIDO);
    buffer_desempaquetar(buffer, regs->ECX, sizeof(*(regs->ECX)) * TAM_REGISTRO_EXTENDIDO);
    buffer_desempaquetar(buffer, regs->EDX, sizeof(*(regs->EDX)) * TAM_REGISTRO_EXTENDIDO);
    buffer_desempaquetar(buffer, regs->RAX, sizeof(*(regs->RAX)) * TAM_REGISTRO_R);
    buffer_desempaquetar(buffer, regs->RBX, sizeof(*(regs->RBX)) * TAM_REGISTRO_R);
    buffer_desempaquetar(buffer, regs->RCX, sizeof(*(regs->RCX)) * TAM_REGISTRO_R);
    buffer_desempaquetar(buffer, regs->RDX, sizeof(*(regs->RDX)) * TAM_REGISTRO_R);
}

void imprimir_un_registro(char* reg, int cantCaracteres) {
    for(int i = 0; i < cantCaracteres; i++){
        printf("%c", reg[i]);
    }
}

void registros_imprimir(registros* regs) {
    printf("\n");
    printf("AX: ");
    imprimir_un_registro(regs->AX, TAM_REGISTRO_GENERAL);
    printf("\n");
    printf("BX: ");
    imprimir_un_registro(regs->BX, TAM_REGISTRO_GENERAL);
    printf("\n");
    printf("CX: ");
    imprimir_un_registro(regs->CX, TAM_REGISTRO_GENERAL);
    printf("\n");
    printf("DX: ");
    imprimir_un_registro(regs->DX, TAM_REGISTRO_GENERAL);
    printf("\n");
    printf("EAX: ");
    imprimir_un_registro(regs->EAX, TAM_REGISTRO_EXTENDIDO);
    printf("\n");
    printf("EBX: ");
    imprimir_un_registro(regs->EBX, TAM_REGISTRO_EXTENDIDO);
    printf("\n");
    printf("ECX: ");
    imprimir_un_registro(regs->ECX, TAM_REGISTRO_EXTENDIDO);
    printf("\n");
    printf("EDX: ");
    imprimir_un_registro(regs->EDX, TAM_REGISTRO_EXTENDIDO);
    printf("\n");
    printf("RAX: ");
    imprimir_un_registro(regs->RAX, TAM_REGISTRO_R);
    printf("\n");
    printf("RBX: ");
    imprimir_un_registro(regs->RBX, TAM_REGISTRO_R);
    printf("\n");
    printf("RCX: ");
    imprimir_un_registro(regs->RCX, TAM_REGISTRO_R);
    printf("\n");
    printf("RDX: ");
    imprimir_un_registro(regs->RDX, TAM_REGISTRO_R);
    printf("\n\n");
}