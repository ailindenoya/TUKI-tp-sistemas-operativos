#include "../include/consola_serializador.h"

void consola_serializador_empaquetar_sin_parametros(t_buffer* buffer, uint8_t instruccion) {
    buffer_empaquetar(buffer, &instruccion, sizeof(instruccion));
}
void consola_serializador_empaquetar_un_parametro(t_buffer* buffer, uint8_t instruccion, char* parametro1) {
    consola_serializador_empaquetar_sin_parametros( buffer, instruccion);
    buffer_empaquetar(buffer, &parametro1, sizeof(parametro1));
}
void consola_serializador_empaquetar_dos_parametros(t_buffer* buffer, uint8_t instruccion, char* parametro1,char* parametro2) {
    consola_serializador_empaquetar_un_parametro( buffer, instruccion, parametro1);
    buffer_empaquetar(buffer, &parametro2, sizeof(parametro2));
}
void consola_serializador_empaquetar_tres_parametros(t_buffer* buffer, uint8_t instruccion,char* parametro1,char* parametro2, char* parametro3) {
consola_serializador_empaquetar_dos_parametros( buffer,instruccion,parametro1,parametro2);
    buffer_empaquetar(buffer, &parametro3, sizeof(parametro3));
}
