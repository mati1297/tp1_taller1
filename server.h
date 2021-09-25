#ifndef TP1_TALLER1_SERVER_H
#define TP1_TALLER1_SERVER_H

#include <stdio.h>
#include "common_socket.h"
#include "common_file_reader.h"
#include "common_hanged.h"

#define MSG_ERROR_OPEN_FILE "Error al abrir el archivo"
#define MSG_ERROR_LISTENING_PORT "Error al poner el puerto en escucha"
#define MSG_ERROR_CONNECTING_CLIENT "Error al conectar con el cliente"
#define MSG_ERROR_PACKING_INFO "Error al empaquetar informacion"
#define MSG_ERROR_SENDING_PACKET "Error al enviar paquete"
#define MSG_ERROR_LETTERS_RECEIVE "Error al recibir letras"
#define MSG_ERROR_INVALID_ATTEMPTS_AMOUNT "Cantidad de intentos invalida"

#define MSG_HANGED_SUMMARY "Resumen"
#define MSG_HANGED_VICTORIES "Victorias"
#define MSG_HANGED_DEFEATS "Derrotas"

typedef struct {
    Socket socket, peer;
    Hanged hanged;
    FileReader file_reader;
} Server;

typedef enum{
    STATE_SUCCESS,
    STATE_ATTEMPTS_AMOUNT_ERROR,
    STATE_FILE_OPENING_ERROR,
    STATE_LISTENING_ERROR,
    STATE_CONNECTING_TO_CLIENT_ERROR,
    STATE_PACKING_INFO_ERROR,
    STATE_SENDING_PACKET_ERROR,
    STATE_RECEIVING_LETTER_ERROR,
} ServerState;

ServerState serverInit(Server * self, char * filename, char * port, char * attempts);
ServerState serverExecute(Server * self);
void serverUnInit(Server * self);
void serverPrintError(ServerState state);


#endif //TP1_TALLER1_SERVER_H
