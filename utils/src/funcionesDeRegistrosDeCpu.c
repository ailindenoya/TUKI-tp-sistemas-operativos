#include "../include/funcionesDeRegistrosDeCpu.h"


void empaquetar_registros(t_buffer* buffer, registros reg){
    buffer_empaquetar(buffer, &reg.AX, sizeof(reg.AX));
    buffer_empaquetar(buffer, &reg.BX, sizeof(reg.BX));
    buffer_empaquetar(buffer, &reg.CX, sizeof(reg.CX));
    buffer_empaquetar(buffer, &reg.DX, sizeof(reg.DX));
    buffer_empaquetar(buffer, &reg.EAX, sizeof(reg.EAX));
    buffer_empaquetar(buffer, &reg.EBX, sizeof(reg.EBX));
    buffer_empaquetar(buffer, &reg.ECX, sizeof(reg.ECX));
    buffer_empaquetar(buffer, &reg.EDX, sizeof(reg.EDX));
    buffer_empaquetar(buffer, &reg.RAX, sizeof(reg.RAX));
    buffer_empaquetar(buffer, &reg.RBX, sizeof(reg.RBX));
    buffer_empaquetar(buffer, &reg.RCX, sizeof(reg.RCX));
    buffer_empaquetar(buffer, &reg.RDX, sizeof(reg.RDX));
}

void desempaquetar_registros(t_buffer* buffer, registros reg){
    buffer_desempaquetar(buffer, &reg.AX, sizeof(reg.AX));
    buffer_desempaquetar(buffer, &reg.BX, sizeof(reg.BX));
    buffer_desempaquetar(buffer, &reg.CX, sizeof(reg.CX));
    buffer_desempaquetar(buffer, &reg.DX, sizeof(reg.DX));
    buffer_desempaquetar(buffer, &reg.EAX, sizeof(reg.EAX));
    buffer_desempaquetar(buffer, &reg.EBX, sizeof(reg.EBX));
    buffer_desempaquetar(buffer, &reg.ECX, sizeof(reg.ECX));
    buffer_desempaquetar(buffer, &reg.EDX, sizeof(reg.EDX));
    buffer_desempaquetar(buffer, &reg.RAX, sizeof(reg.RAX));
    buffer_desempaquetar(buffer, &reg.RBX, sizeof(reg.RBX));
    buffer_desempaquetar(buffer, &reg.RCX, sizeof(reg.RCX));
    buffer_desempaquetar(buffer, &reg.RDX, sizeof(reg.RDX));
}