#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <netdb.h>
#include "common_socket.h"


/* Funcion estatica que a partir de direcciones de host y de port
 * crean e inicializan una estructura de tipo struct addrinfo con
 * las direcciones deseadas. Si host fuera NULL se toma como que
 * se inicializa un socket de un server.  */
static uint8_t _socketGetAddressInfo(struct addrinfo ** result,
                                     char * host, char * port){
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (host == NULL)
        hints.ai_flags = AI_PASSIVE;
    int s = getaddrinfo(host, port, &hints, result);
    if (s) {
        freeaddrinfo(*result);
        return 1;
    }
    return 0;
}

void socketInit(Socket * self){
    self->fd = INVALID_FILE_DESCRIPTOR;
}
// Esto deberia ser static, lo usas solo en el accept, sino podrias leakear el fd
void socketInitFromFd(Socket * self, int fd){
    self->fd = fd;
}

void socketUnInit(Socket * self){
    if (self->fd != INVALID_FILE_DESCRIPTOR){
        close(self->fd);
        self->fd = INVALID_FILE_DESCRIPTOR;
    }
}

uint8_t socketConnect(Socket * self, char * host, char * port){
    struct addrinfo * result, * ptr;

    // Se crea e inicializa al estructura de direccion.
    if (_socketGetAddressInfo(&result, host, port)) {
        freeaddrinfo(result);
        return 1;
    }

    // Se itera a traves de los resultados para encontrar a cual
    // se puede conectar.
    for (ptr = result; ptr; ptr = ptr->ai_next){
        int skt = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (skt != -1) {
            // Si la conexion falla se cierra el socket, sino se
            // guarda el file descriptor.
            if (connect(skt, ptr->ai_addr, ptr->ai_addrlen)) {
                close(skt);
            } else {
                self->fd = skt;
                break;
            }
        }
    }

    // Se libera el puntero a la estructura.
    freeaddrinfo(result);
    // Si no se guardo ningun file descriptor es que no se pudo
    // conectar a ningun lado, por lo que la funcion fallo.
    if (self->fd == INVALID_FILE_DESCRIPTOR)
        return 1;

    // Se setea el socket para que pueda reutilizar la direccion.
    int optval = 1;
    setsockopt(self->fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    return 0;
}

uint8_t socketBindAndListen(Socket * self, char * port){
    struct addrinfo * result;

    // Se crea e inicializa al estructura de direccion. Pasandole como
    // host NULL ya que se quiere inicializar un server.
    if (_socketGetAddressInfo(&result, NULL, port))
        return 1;

    // Se inicializa el file descriptor del socket.
    self->fd = socket(result->ai_family, result->ai_socktype,
                      result->ai_protocol);

    // Se setea el socket para que pueda reutilizar la direccion.
    int optval = 1;
    setsockopt(self->fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

    // Se realiza el bind.
    // Aca tambien tenes que loopear por addrinfo, como en el connect
    if (bind(self->fd, result->ai_addr, result->ai_addrlen)) {
        freeaddrinfo(result);
        close(self->fd);
        self->fd = INVALID_FILE_DESCRIPTOR;
        return 1;
    }
    // Se libera el puntero a la estructura.
    freeaddrinfo(result);

    // Se deja el socket en escucha.
    if (listen(self->fd, SERVER_PENDING_CONNECTIONS))
        return 1;

    return 0;
}

uint8_t socketAccept(Socket * self, Socket * peer){
    // Se acepta una conexion y se guarda el file descriptor
    // del peer.
    int fd_peer = accept(self->fd, NULL, NULL);
    if (fd_peer == -1)
        return 1;
    // Se inicializa peer con el file descriptor obtenido de
    // la funcion accept.
    socketInitFromFd(peer, fd_peer);
    return 0;
}

ssize_t socketSend(Socket * self, char * buffer, size_t size){
    size_t total_bytes_sent = 0;
    // Se realiza un ciclo hasta que todos los bytes sean enviados
    while ((size - total_bytes_sent) > 0){
        // Se envian los datos que quedan enviar a traves del socket.
        int bytes_sent = send(self->fd, &buffer[total_bytes_sent],
                              size - total_bytes_sent, MSG_NOSIGNAL);
        // Si se cerro el socket o hubo un error, se retorna -1
        // indicando error.
        if (bytes_sent == 0 || bytes_sent == -1)
            return -1;
        total_bytes_sent += bytes_sent;
    }
    return total_bytes_sent;
}

ssize_t socketReceive(Socket * socket, char * buffer, size_t size){
    size_t total_bytes_received = 0;
    // Se realiza un ciclo hasta que todos los bytes sean recibidos.
    while ((size - total_bytes_received) > 0){
        // Se reciben los datos a traves del socket.
        int bytes_received =
                recv(socket->fd, &buffer[total_bytes_received],
                     size - total_bytes_received, 0);
        // Si hubo un error, se retorna -1 indicando error.
        if (bytes_received == -1)
            return -1;
        // Si se cerro el socket, se retorna los bytes recibidos hasta
        // ahora. Queda por cuenta de quien use la funcion verificarlo
        // si es necesario.
        else if (!bytes_received)
            return total_bytes_received;
        total_bytes_received += bytes_received;
    }
    return total_bytes_received;
}


