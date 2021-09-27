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

// Tipo Socket, permite enviar y recibir datos a traves de
// sockets.
typedef struct {
    int fd;
} Socket;

/* Inicializa la instancia self.
 * Pre: self debe apuntar a una direccion de memoria valida.  */
void socketInit(Socket * self);

/* Inicializa la instacia self a partir de un file descriptor.
 * Pre: self debe apuntar a una direccion de memoria valida.
 *      fd debe contener un file descriptor valido.  */
void socketInitFromFd(Socket * self, int fd);

/* Desinicializa la instancia self.
 * Pre: self debe apuntar a una direccion de memoria valida.  */
void socketUnInit(Socket * self);

/* Conecta el socket al host y puerto indicados. Retorna 0 en exito o 1
 * en caso de error.
 * Pre: self debe apuntar a una direccion de memoria valida.
 *      host debe apuntar a un array de chars con una direccion de host
 *      valida.
 *      port debe apuntar a un array de chars con una direccion de puerto
 *      valida.  */
uint8_t socketConnect(Socket * self, char * host, char * port);

/* Asigna una direccion al socket y luego lo pone en escucha. Retorna 0
 * en caso de exito o 1 en caso de error.
 * Pre: self debe apuntar a una direccion de memoria valida.
 *      port debe apuntar a un array de chars con una direccion de puerto
 *      valida.  */
uint8_t socketBindAndListen(Socket * self, char * port);

/* Acepta una conexion e inicializa peer con el socket correspondiente
 * al peer. Retorna 0 en caso de exito o 1 en caso de error.
 * Pre: self debe apuntar a una direccion de memoria valida.
 *      peer debe apuntar a una direccion de memoria valida.  */
uint8_t socketAccept(Socket * self, Socket * peer);

/* Envia la cantidad de bytes del buffer indicada por size. Retorna
 * la cantidad de bytes enviados o -1 en caso de error.
 * Pre: self debe apuntar a una direccion de memoria valida.
 *      buffer debe apuntar a un array de chars con al menos
 *      size bytes.  */
ssize_t socketSend(Socket * self, char * buffer, size_t size);

/* Recibe la cantidad de datos indicada por size y los guarda en buffer.
 * Retorna la candida de bytes recibidos o -1 en caso de error. Si el
 * socket fue cerrado, se lee y se guarda la cantidad que se pudo leer.
 * Pre: self debe apuntar a una direccion de memoria valida.
 *      buffer debe apuntar a un array de chars con al menos size bytes.  */
ssize_t socketReceive(Socket * self, char * buffer, size_t size);

#endif //TP1_TALLER1_COMMON_SOCKET_H
