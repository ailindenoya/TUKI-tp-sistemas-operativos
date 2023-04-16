#include "../include/conexiones.h"


int iniciar_servidor(char* ip, char* port) {
    int optVal = 1;
    struct addrinfo hints;
    struct addrinfo* serverInfo;

    memset(&hints, 0, sizeof(hints));


    hints.ai_family = AF_UNSPEC; ///* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE; //random  IP address */

    int rv = getaddrinfo(ip, port, &hints, &serverInfo);
    if (rv != 0) {
        printf("getaddrinfo error: %s\n", gai_strerror(rv));
        return EXIT_FAILURE;
    }

    int serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
    if (-1 == serverSocket) {	
        printf("error al crear socket \n%s", strerror(errno));
        return EXIT_FAILURE;
    }

    int sockOpt = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal));
    if (-1 == sockOpt) {
        printf("error socketOpt \n%s", strerror(errno));
        return EXIT_FAILURE;
    }   

// The setsockopt() function sets options associated with a socket. 
//Options can exist at multiple protocol levels.

    int bindVal = bind(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
    if (-1 == bindVal) {
        printf("error de bind mismatch\n%s", strerror(errno));
        return EXIT_FAILURE;
    }

    int listenVal = listen(serverSocket, SOMAXCONN);
    if (-1 == listenVal) {
        printf("error de listen \n%s", strerror(errno));
        return EXIT_FAILURE;
    }

    freeaddrinfo(serverInfo);

    return serverSocket;
}


int conectar_a_servidor(char* ip, char* port) {
    int conexion;
    int status;
    struct addrinfo hints;
    struct addrinfo* servidor; // puntero a los resultados
    struct addrinfo* p; // para no perder el valor de servidor

    memset(&hints, 0, sizeof(hints));// asegura que la struct no este vacia

    // hints para establecer protocolos:

    hints.ai_family = AF_UNSPEC;  // ipv4 - ipv6 para usar uno en especifico:  AF_INET o AF_INET6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // pone el ip random automaticamente

    status = getaddrinfo(ip, port, &hints, &servidor);

    if ( status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    // servidor ahora apunta a una lista de 1 o mas structs de addrinfo

    
    for (p = servidor; p != NULL; p = p->ai_next) {
        conexion = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (-1 == conexion) {
            continue;
        }
        if (connect(conexion, p->ai_addr, p->ai_addrlen) != -1) {
            break;
        }
        close(conexion);
    }
    freeaddrinfo(servidor);
    if (conexion != -1 && p != NULL) {
        return conexion;
    }
    return -1;
}
