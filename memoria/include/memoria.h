#ifndef MEMORIA_H_INCLUDED
#define MEMORIA_H_INCLUDED



struct hueco_libre{
    int direccion;     
    int tamanio; 
};

typedef struct hueco_libre hueco_libre;

void recibir_de_kernel(void);
hueco_libre* crear_hueco_libre(int tamanio, int dir);
void atender_delete_segment(int pid, int idSegmento );
void atender_create_segment(int pid, int idSegmento);
void sumar_tamanio_al_libre_total(hueco_libre* hueco);
void enviar_cant_segmentos_a_kernel(void);
void avisar_si_hay_error(int socket, char* tipo);
void handshake_kernel(void);
void handshake_cpu(int socketCPU);
void handshake_filesystem(int socketFilesystem);
bool ver_si_tamanio_requerido_entra_en_hueco_libre(void* huecoAux);
hueco_libre* best_fit(void);
hueco_libre* worst_fit(void);
hueco_libre* first_fit(void);
hueco_libre* obtener_hueco_mas_grande(hueco_libre* hueco1, hueco_libre* hueco2);
hueco_libre* obtener_hueco_mas_chico(hueco_libre* hueco1, hueco_libre* hueco2);

#endif