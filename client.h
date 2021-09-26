#ifndef TP1_TALLER1_CLIENT_H
#define TP1_TALLER1_CLIENT_H

#include "common_socket.h"
#include "common_hanged.h"

#define MSG_ERROR_ARGS_AMOUNT "La cantidad de argumentos debe ser"
#define MSG_ERROR_INVALID_PORT_NUMBER "Numero de puerto invalido"
#define MSG_ERROR_SERVER_CONNECTION "Error al conectar al server"
#define MSG_ERROR_PACKAGE_RECEIVE "Error al recibir el paquete de información"
#define MSG_ERROR_READING_LETTERS "Error al leer las letras de entrada"
#define MSG_ERROR_SENDING_LETTERS "Error al enviar los datos de letra"
#define MSG_ERROR_INVALID_LETTER "Letra invalida, ingresa otra"

#define MSG_HANGED_SECRET_WORD "Palabra secreta"
#define MSG_HANGED_ATTEMPTS_PARTONE "Te quedan"
#define MSG_HANGED_ATTEMPTS_PARTTWO "intentos"
#define MSG_HANGED_NEW_LETTER "Ingrese letra"
#define MSG_HANGED_YOU_WIN "Ganaste!!"
#define MSG_HANGED_YOU_LOSE "Perdiste! La palabra secreta era"

typedef struct {
    Socket socket;
    FileReader file_reader;
} Client;

typedef enum{
    STATE_SUCCESS,
    STATE_CONNECTION_ERROR,
    STATE_RECEIVING_PACKET_ERROR,
    STATE_READING_STDIN_ERROR,
    STATE_SENDING_LETTER_ERROR
} ClientState;

void clientInit(Client * self);
ClientState clientConnect(Client * self, char * host, char * port);
ClientState clientExecute(Client * self);
void clientUnInit(Client * self);
void clientPrintError(ClientState state);

#endif //TP1_TALLER1_CLIENT_H
