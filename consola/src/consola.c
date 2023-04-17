#include "../include/consola_config.h"


#define LOGS_CONSOLA "bin/consola.log"
#define MODULO_CONSOLA "Consola"
#define NUMBER_OF_ARGS_REQUIRED 3 


static void consola_destruir(t_log *consolaLogger,t_consola_config *consolaConfig ){
    log_destroy(consolaLogger);
    consola_config_destruir(consolaConfig);
}


int main(int argc, char *argv[]) {

    t_log *consolaLogger = log_create(LOGS_CONSOLA, MODULO_CONSOLA, true, LOG_LEVEL_INFO);
    
    t_consola_config *consolaConfig = consola_crear_config(argv[1], consolaLogger);

    if (argc != NUMBER_OF_ARGS_REQUIRED) {
        log_error(consolaLogger, "Cantidad de argumentos inválida.\nArgumentos: <tamañoProceso> <pathInstrucciones>");
        consola_destruir(consolaLogger, consolaConfig);
        return -1;
    }
    
    // obtenemos IP y PUERTO de kernel
    char *kernelIP = consola_config_obtener_kernel_IP(consolaConfig);
    char *kernelPUERTO = consola_config_obtener_kernel_PUERTO(consolaConfig);

    // creamos socket
    const int kernelSocket = conectar_a_servidor(kernelIP, kernelPUERTO);
    if (kernelSocket == -1) {
        log_error(consolaLogger, "Consola no se pudo conectar con Kernel");
        consola_destruir(consolaLogger, consolaConfig);
        return -1;
    }

    consola_config_destruir(consolaConfig);
    log_destroy(consolaLogger);
    return 0;

}





