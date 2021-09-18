//
// Created by matias on 16/9/21.
//

#ifndef TP1_TALLER1_COMMON_SOCKET_H
#define TP1_TALLER1_COMMON_SOCKET_H

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    int fd;
    struct sockaddr_in addr;
} Socket;

void socketInit(Socket * self);
void socketInitFromFd(Socket * self, int fd);
void socketUnInit(Socket * self);
int socketConnect(Socket * self, char * host, int port);
int socketBindAndListen(Socket * self, int port);
int socketAccept(Socket * self, Socket * peer);
int socketSend(Socket * self, char * buffer, size_t size);
int socketReceive(Socket * socket, char * buffer, size_t size);

#endif //TP1_TALLER1_COMMON_SOCKET_H
