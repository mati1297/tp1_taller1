#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <netdb.h>
#include "common_socket.h"



static uint8_t _socketGetAddressInfo(struct addrinfo ** result,
                                     char * host, char * port){
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (host == NULL)
        hints.ai_flags = AI_PASSIVE;
    int s = getaddrinfo(host, port, &hints, result);
    if (s)
        return 1;
    return 0;
}

void socketInit(Socket * self){
    self->fd = 0;
}

void socketInitFromFd(Socket * self, int fd){
    self->fd = fd;
}

void socketUnInit(Socket * self){
    if(self->fd){
        close(self->fd);
        self->fd = 0;
    }
}

uint8_t socketConnect(Socket * self, char * host, char * port){
    struct addrinfo * result, * ptr;
    if (_socketGetAddressInfo(&result, host, port)) {
        return 1;
    }
    for (ptr = result; ptr; ptr = ptr->ai_next){
        int skt = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (skt != -1) {
            if (connect(skt, ptr->ai_addr, ptr->ai_addrlen)) {
                close(skt);
            } else {
                self->fd = skt;
                break;
            }
        }
    }
    freeaddrinfo(ptr);
    if (!self->fd)
        return 1;

    int optval = 1;
    setsockopt(self->fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    return 0;
}

uint8_t socketBindAndListen(Socket * self, char * port){
    struct addrinfo * result;
    if (_socketGetAddressInfo(&result, NULL, port))
        return 1;
    self->fd = socket(result->ai_family, result->ai_socktype,
                      result->ai_protocol);
    int optval = 1;
    setsockopt(self->fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

    if (bind(self->fd, result->ai_addr, result->ai_addrlen)) {
        freeaddrinfo(result);
        close(self->fd);
        self->fd = 0;
        return 1;
    }
    freeaddrinfo(result);

    if (listen(self->fd, SERVER_PENDING_CONNECTIONS))
        return 1;

    return 0;
}

uint8_t socketAccept(Socket * self, Socket * peer){
    int fd_peer = accept(self->fd, NULL, NULL);
    if (fd_peer < 0)
        return 1;
    socketInitFromFd(peer, fd_peer);
    return 0;
}

ssize_t socketSend(Socket * socket, char * buffer, size_t size){
    size_t total_bytes_sent = 0;
    while ((size - total_bytes_sent) > 0){
        int bytes_sent = send(socket->fd, &buffer[total_bytes_sent],
                              size - total_bytes_sent, MSG_NOSIGNAL);
        if (bytes_sent == 0)
            return -1;
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


