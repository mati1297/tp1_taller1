#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "common_socket.h"
// VER LO QUE ESTABA ESCRITO EN LA CARPETA

#define PENDING_CONNECTIONS 8
#define LOCALHOST "localhost"

static uint8_t _socketInitializeSocketAddressStruct(Socket * self, char * host, uint16_t port){
    memset(&(self->address), 0, sizeof(struct  sockaddr_in));
    (self->address).sin_family = AF_INET;
    (self->address).sin_port = htons(port);
    if (host && strcmp(host, LOCALHOST)){
        if (!inet_pton(AF_INET, host, &(self->address).sin_addr))
            return 1;
    }
    return 0;
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

uint8_t socketConnect(Socket * self, char * host, uint16_t port){
    if(_socketInitializeSocketAddressStruct(self, host, port))
        return 1;
    if (connect(self->fd,
                (struct sockaddr *) &(self->address), sizeof(struct sockaddr)))
        return 1;
    return 0;
}

uint8_t socketBindAndListen(Socket * self, uint16_t port){
    _socketInitializeSocketAddressStruct(self, NULL, port);
    if (bind(self->fd,
             (struct sockaddr *) &(self->address), sizeof(struct sockaddr))) {
        return 1;
    }
    if (listen(self->fd, PENDING_CONNECTIONS)) {
        return 1;
    }
    return 0;
}

uint8_t socketAccept(Socket * self, Socket * peer){
    socklen_t peer_addr_size = sizeof(struct sockaddr_in);
    int fd_peer = accept(self->fd,
                         (struct sockaddr *) &(self->address), &peer_addr_size);
    if (fd_peer < 0)
        return 1;
    socketInitFromFd(peer, fd_peer);
    return 0;
}

ssize_t socketSend(Socket * socket, char * buffer, size_t size){
    size_t total_bytes_sent = 0;
    while ((size - total_bytes_sent) > 0){
        int bytes_sent = send(socket->fd, &buffer[total_bytes_sent],
                              size - total_bytes_sent, 0);
        if (bytes_sent == -1)
            return -1;
        total_bytes_sent += bytes_sent;
    }
    return total_bytes_sent;
}

ssize_t socketReceive(Socket * socket, char * buffer, size_t size){
    size_t total_bytes_received = 0;
    while ((size - total_bytes_received) > 0){

        int bytes_received =
                recv(socket->fd, &buffer[total_bytes_received],
                     size - total_bytes_received, 0);
        if (bytes_received == -1)
            return -1;
        else if (!bytes_received)
            return total_bytes_received;
        total_bytes_received += bytes_received;
    }
    return total_bytes_received;
}


