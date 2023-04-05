#include <stdlib.h>
#include <stdio.h>
#include <commons/collections/list.h>
#include <commons/log.h>


char *mi_funcion_compartida(){
    return "holaaa";
}

int main(int argc, char* argv[]) {
    puts("Hello world!!");
    
    t_list *names = list_create();
    list_add(names, "Mick");
    list_add(names, "Keith");
    list_add(names, "Ronnie");
    list_add(names, "Charlie");

    list_iterate(names, (void *)puts);


    t_log* logger = log_create("proceso1.log", "PROCESO1", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy el proceso 1! %s", mi_funcion_compartida());
    log_destroy(logger);
    
    return 0;
    

}
