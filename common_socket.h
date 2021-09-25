//
// Created by matias on 16/9/21.
//

#ifndef TP1_TALLER1_COMMON_SOCKET_H
#define TP1_TALLER1_COMMON_SOCKET_H

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <netdb.h>

#define SERVER_PENDING_CONNECTIONS 8

typedef struct {
    int fd;
} Socket;

void socketInit(Socket * self);
void socketInitFromFd(Socket * self, int fd);
void socketUnInit(Socket * self);
uint8_t socketConnect(Socket * self, char * host, char * port);
uint8_t socketBindAndListen(Socket * self, char * port);
uint8_t socketAccept(Socket * self, Socket * peer);
ssize_t socketSend(Socket * self, char * buffer, size_t size);
ssize_t socketReceive(Socket * socket, char * buffer, size_t size);

#endif //TP1_TALLER1_COMMON_SOCKET_H
