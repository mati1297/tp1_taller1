//
// Created by matias on 23/9/21.
//

#ifndef TP1_TALLER1_SERVER_H
#define TP1_TALLER1_SERVER_H

#include <stdio.h>
#include "common_socket.h"
#include "common_file_reader.h"
#include "common_hanged.h"
#include "server.h"

#define MAX_ERROR_MSG_LENGTH 20

// Macros de texto

#define MSG_ERROR_OPEN_FILE "Error al abrir el archivo"
#define MSG_ERROR_LISTENING_PORT "Error al poner el puerto en escucha"
#define MSG_ERROR_CONNECTING_CLIENT "Error al conectar con el cliente"
#define MSG_ERROR_PACKING_INFO "Error al empaquetar informacion"
#define MSG_ERROR_SENDING_PACKET "Error al enviar paquete"
#define MSG_ERROR_LETTERS_RECEIVE "Error al recibir letras"
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
    STATE_FILE_OPENING_ERROR,
    STATE_LISTENING_ERROR,
    STATE_CONNECTING_TO_CLIENT_ERROR,
    STATE_PACKING_INFO_ERROR,
    STATE_SENDING_PACKET_ERROR,
    STATE_RECEIVING_LETTER_ERROR,
} ServerState;

ServerState serverInit(Server * self, char * filename, uint16_t port, uint16_t attempts);
ServerState serverExecute(Server * self);
HangedState serverGetPlayerVictories(Server * self);
HangedState serverGetPlayerDefeats(Server * self);
void serverUnInit(Server * self);
void serverGetError(ServerState state, char * buffer, size_t size);


#endif //TP1_TALLER1_SERVER_H
