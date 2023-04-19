#include "../include/consola_config.h"


#define LOGS_CONSOLA "bin/consola.log"
#define MODULO_CONSOLA "Consola"
#define NUMERO_DE_ARGUMENTOS_NECESARIOS 3 


static void consola_destruir(t_log *consolaLogger,t_consola_config *consolaConfig ){
    log_destroy(consolaLogger);
    consola_config_destruir(consolaConfig);
}


int main(int argc, char *argv[]) {

    t_log *consolaLogger = log_create(LOGS_CONSOLA, MODULO_CONSOLA, true, LOG_LEVEL_INFO);
    
    t_consola_config *consolaConfig = consola_crear_config(argv[1], consolaLogger);

    if (argc != NUMERO_DE_ARGUMENTOS_NECESARIOS) {
        log_error(consolaLogger, "Cantidad de argumentos inválida.\nArgumentos: <tamañoProceso> <pathInstrucciones>");
        consola_destruir(consolaLogger, consolaConfig);
        return -1;
    }
    
    // conexion con KERNEL

    const int kernelSocket = conectar_a_servidor(consola_config_obtener_kernel_IP(consolaConfig), consola_config_obtener_kernel_PUERTO(consolaConfig));
    if (kernelSocket == -1) {
        log_error(consolaLogger, "Consola no se pudo conectar con Kernel");
        consola_destruir(consolaLogger, consolaConfig);
        return -1;
    }

    consola_config_destruir(consolaConfig);
    log_destroy(consolaLogger);
    return 0;

}





