#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "common_socket.h"
// VER LO QUE ESTABA ESCRITO EN LA CARPETA

#define PENDING_CONNECTIONS 8

static void _socketInitializeSockAddrInStruct(Socket * self, int port){
    memset(&(self->addr), 0, sizeof(struct  sockaddr_in));
    (self->addr).sin_family = AF_INET;
    (self->addr).sin_port = htons(port);
}

void socketInit(Socket * self){
    self->fd = socket(AF_INET, SOCK_STREAM, 0);
    int optval = 1;
    setsockopt(self->fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
}

void socketInitFromFd(Socket * self, int fd){
    self->fd = fd;
    int optval = 1;
    setsockopt(self->fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
}

void socketUnInit(Socket * self){
    shutdown(self->fd, SHUT_WR); // BUSCAR ESTO DE SHUT
    close(self->fd);
}

int socketConnect(Socket * self, char * host, int port){
    _socketInitializeSockAddrInStruct(self, port);
    if (inet_pton(AF_INET, host, &(self->addr).sin_addr))
        return 1;
    if (connect(self->fd,
                (struct sockaddr *) &(self->addr), sizeof(struct sockaddr)))
        return 1;
    return 0;
}

int socketBindAndListen(Socket * self, int port){
    _socketInitializeSockAddrInStruct(self, port);
    if (bind(self->fd,
             (struct sockaddr *) &(self->addr), sizeof(struct sockaddr))) {
        return 1;
    }
    if (listen(self->fd, PENDING_CONNECTIONS)) {
        return 1;
    }
    return 0;
}

int socketAccept(Socket * self, Socket * peer){
    socklen_t peer_addr_size = sizeof(struct sockaddr_in);
    int fd_peer = accept(self->fd,
                         (struct sockaddr *) &(self->addr), &peer_addr_size);
    if (fd_peer < 0)
        return 1;
    socketInitFromFd(peer, fd_peer);
    return 0;
}


//REVISAR FUNCIONES CON LO DEL DIPA
int socketSend(Socket * socket, char * buffer, size_t size){
    size_t total_bytes_sent = 0;
    while ((size - total_bytes_sent) > 0){
        int bytes_sent = send(socket->fd, &buffer[total_bytes_sent],
                              size - total_bytes_sent, 0);
        if (bytes_sent < 0)
            return -1;
        total_bytes_sent += bytes_sent;
    }
    return total_bytes_sent;
}

int socketReceive(Socket * socket, char * buffer, size_t size){
    size_t total_bytes_received = 0;
    while ((size - total_bytes_received) > 0){

        int bytes_received =
                recv(socket->fd, &buffer[total_bytes_received],
                     size - total_bytes_received, 0);
        if (bytes_received < 0)
            return -1;
        else if (!bytes_received)
            return total_bytes_received;
        total_bytes_received += bytes_received;
    }
    return total_bytes_received;
}


