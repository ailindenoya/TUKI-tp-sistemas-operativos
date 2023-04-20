#include "../include/consola_parser.h"

#define MAX_LENGTH_INSTRUCTION 6

bool consola_parsear_instrucciones(t_buffer *buffer, const char *pathInstrucciones, t_log *consolaLogger) {
    bool parseSuccess = false;
    FILE *archivoInstrucciones = fopen(pathInstrucciones, "r");
    uint32_t parametro1 = -1;
    uint32_t parametro2 = -1;
    uint32_t parametro3 = -1;

    char *instruccion = malloc(MAX_LENGTH_INSTRUCTION);
    char *fscanfError = NULL;
    char const *const fscanfErrorPrefix = "Error al intentar parsear";
    for (;;) {
        if (fscanf(archivoInstrucciones, "%s", instruccion)) {
            bool hayUnParametro = false;
            bool hayDosParametros = false;
            bool hayTresParametros = false;
            bool shouldFail = false;
            if (strcmp(instruccion, "F_READ") == 0) {
                if (fscanf(archivoInstrucciones, "%d %d %d", &parametro1, &parametro2, &parametro3)) {
                   consola_serializador_empaquetar_tres_parametros(buffer,INSTRUCCION_f_read, parametro1, parametro2, parametro3);
                   hayTresParametros = true;
                } else {
                    shouldFail = true;
                    string_from_format(fscanfError, "%s argumentos de F_READ: %s", fscanfErrorPrefix, strerror(errno));
                }
            } else if (strcmp(instruccion, "F_WRITE") == 0) {
                if (fscanf(archivoInstrucciones, "%d %d %d", &parametro1, &parametro2, &parametro3)) {
                   consola_serializador_empaquetar_tres_parametros(buffer,INSTRUCCION_f_write, parametro1, parametro2, parametro3);
                   hayTresParametros = true;
                } else {
                    shouldFail = true;
                    string_from_format(fscanfError, "%s argumentos de F_WRITE: %s", fscanfErrorPrefix, strerror(errno));
                }
            } else if (strcmp(instruccion, "SET") == 0) {
                if (fscanf(archivoInstrucciones, "%d %d", &parametro1, &parametro2)) {
                   consola_serializador_empaquetar_dos_parametros(buffer,INSTRUCCION_set, parametro1, parametro2);
                   hayDosParametros = true;
                } else {
                    shouldFail = true;
                    string_from_format(fscanfError, "%s argumentos de SET: %s", fscanfErrorPrefix, strerror(errno));
                }
            } else if (strcmp(instruccion, "MOV_IN") == 0) {
                if (fscanf(archivoInstrucciones, "%d %d", &parametro1, &parametro2)) {
                   consola_serializador_empaquetar_dos_parametros(buffer,INSTRUCCION_mov_in, parametro1, parametro2);
                   hayDosParametros = true;
                } else {
                    shouldFail = true;
                    string_from_format(fscanfError, "%s argumentos de MOV_IN: %s", fscanfErrorPrefix, strerror(errno));
                }
            } else if (strcmp(instruccion, "MOV_OUT") == 0) {
                if (fscanf(archivoInstrucciones, "%d %d", &parametro1, &parametro2)) {
                   consola_serializador_empaquetar_dos_parametros(buffer,INSTRUCCION_mov_out, parametro1, parametro2);
                   hayDosParametros = true;
                } else {
                    shouldFail = true;
                    string_from_format(fscanfError, "%s argumentos de MOV_OUT: %s", fscanfErrorPrefix, strerror(errno));
                }
            } else if (strcmp(instruccion, "F_TRUNCATE") == 0) {
                if (fscanf(archivoInstrucciones, "%d %d", &parametro1, &parametro2)) {
                   consola_serializador_empaquetar_dos_parametros(buffer,INSTRUCCION_f_truncate, parametro1, parametro2);
                   hayDosParametros = true;
                } else {
                    shouldFail = true;
                    string_from_format(fscanfError, "%s argumentos de F_TRUNCATE: %s", fscanfErrorPrefix, strerror(errno));
                }
            } else if (strcmp(instruccion, "F_SEEK") == 0) {
                if (fscanf(archivoInstrucciones, "%d %d", &parametro1, &parametro2)) {
                   consola_serializador_empaquetar_dos_parametros(buffer,INSTRUCCION_f_seek, parametro1, parametro2);
                   hayDosParametros = true;
                } else {
                    shouldFail = true;
                    string_from_format(fscanfError, "%s argumentos de F_SEEK: %s", fscanfErrorPrefix, strerror(errno));
                }
            } else if (strcmp(instruccion, "CREATE_SEGMENT") == 0) {
                if (fscanf(archivoInstrucciones, "%d %d", &parametro1, &parametro2)) {
                   consola_serializador_empaquetar_dos_parametros(buffer,INSTRUCCION_f_create_segment, parametro1, parametro2);
                   hayDosParametros = true;
                } else {
                    shouldFail = true;
                    string_from_format(fscanfError, "%s argumentos de CREATE_SEGMENT: %s", fscanfErrorPrefix, strerror(errno));
                }
            } else if (strcmp(instruccion, "I/O") == 0) {
                if (fscanf(archivoInstrucciones, "%d", &parametro1)) {
                   consola_serializador_empaquetar_un_parametro(buffer,INSTRUCCION_io, parametro1);
                   hayUnParametro = true;
                } else {
                    shouldFail = true;
                    string_from_format(fscanfError, "%s argumentos de I/O: %s", fscanfErrorPrefix, strerror(errno));
                }
            } else if (strcmp(instruccion, "WAIT") == 0) {
                if (fscanf(archivoInstrucciones, "%d", &parametro1)) {
                   consola_serializador_empaquetar_un_parametro(buffer,INSTRUCCION_wait, parametro1);
                   hayUnParametro = true;
                } else {
                    shouldFail = true;
                    string_from_format(fscanfError, "%s argumentos de WAIT: %s", fscanfErrorPrefix, strerror(errno));
                }
            } else if (strcmp(instruccion, "SIGNAL") == 0) {
                if (fscanf(archivoInstrucciones, "%d", &parametro1)) {
                   consola_serializador_empaquetar_un_parametro(buffer,INSTRUCCION_signal, parametro1);
                   hayUnParametro = true;
                } else {
                    shouldFail = true;
                    string_from_format(fscanfError, "%s argumentos de SIGNAL: %s", fscanfErrorPrefix, strerror(errno));
                }
            } else if (strcmp(instruccion, "F_OPEN") == 0) {
                if (fscanf(archivoInstrucciones, "%d", &parametro1)) {
                   consola_serializador_empaquetar_un_parametro(buffer,INSTRUCCION_f_open, parametro1);
                   hayUnParametro = true;
                } else {
                    shouldFail = true;
                    string_from_format(fscanfError, "%s argumentos de F_OPEN: %s", fscanfErrorPrefix, strerror(errno));
                }
            } else if (strcmp(instruccion, "F_CLOSE") == 0) {
                if (fscanf(archivoInstrucciones, "%d", &parametro1)) {
                   consola_serializador_empaquetar_un_parametro(buffer,INSTRUCCION_f_close, parametro1);
                   hayUnParametro = true;
                } else {
                    shouldFail = true;
                    string_from_format(fscanfError, "%s argumentos de F_CLOSE: %s", fscanfErrorPrefix, strerror(errno));
                }
            } else if (strcmp(instruccion, "DELETE_SEGMENT") == 0) {
                if (fscanf(archivoInstrucciones, "%d", &parametro1)) {
                   consola_serializador_empaquetar_un_parametro(buffer,INSTRUCCION_delete_segment, parametro1);
                   hayUnParametro = true;
                } else {
                    shouldFail = true;
                    string_from_format(fscanfError, "%s argumentos de DELETE_SEGMENT: %s", fscanfErrorPrefix, strerror(errno));
                }
            } else if (strcmp(instruccion, "YIELD") == 0) {
                // VER ESTO 
                consola_serializador_empaquetar_sin_parametros(buffer, INSTRUCCION_yield);
                log_info(consolaLogger, "Se empaqueta instruccion: YIELD");

            }
            else if (strcmp(instruccion, "EXIT") == 0) {
                consola_serializador_empaquetar_sin_parametros(buffer, INSTRUCCION_exit);
                log_info(consolaLogger, "Se empaqueta instruccion: EXIT");
                parseSuccess = true;
                break;
            } else {
                log_error(consolaLogger, "Instruccion no reconocida");
                break;
            }
            if (shouldFail) {
                log_error(consolaLogger, "%s", fscanfError);
                free(fscanfError);
                exit(-1);
            } else if(hayTresParametros){
                      log_info(consolaLogger, "Se empaqueta instruccion: %s con operandos %d , %d y %d", instruccion, parametro1, parametro2, parametro3);
            } else if (hayDosParametros) {
                      log_info(consolaLogger, "Se empaqueta instruccion: %s con operandos %d y %d", instruccion, parametro1, parametro2);
            } else if (hayUnParametro) { 
                      log_info(consolaLogger, "Se empaqueta instruccion: %s con operando %d", instruccion, parametro1);
            } 
            else {

                log_info(consolaLogger, "Se empaqueta instruccion: %s sin operandos", instruccion);

            }
        } else {
            log_error(consolaLogger, "Error al leer el archivo %s de instrucciones: %s", pathInstrucciones, strerror(errno));
            break;
        }
    }
    
    free(instruccion);
    fclose(archivoInstrucciones);
    return parseSuccess;
}