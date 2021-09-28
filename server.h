#ifndef TP1_TALLER1_SERVER_H
#define TP1_TALLER1_SERVER_H

#include <stdio.h>
#include "common_socket.h"
#include "common_file_reader.h"
#include "common_hanged.h"

#define INFORMATION_PACK_HEADER_SIZE 3
#define MASK_ATTEMPTS 127
#define MASK_STATE 128

#define MSG_ERROR_OPEN_FILE "Error al abrir el archivo"
#define MSG_ERROR_LISTENING_PORT "Error al poner el puerto en escucha"
#define MSG_ERROR_CONNECTING_CLIENT "Error al conectar con el cliente"
#define MSG_ERROR_PACKING_INFO "Error al empaquetar informacion"
#define MSG_ERROR_SENDING_PACKET "Error al enviar paquete"
#define MSG_ERROR_LETTERS_RECEIVE "Error al recibir letras"
#define MSG_ERROR_INVALID_ATTEMPTS_AMOUNT "Cantidad de intentos invalida"
#define MSG_ERROR_READING_WORD "Error al leer la palabra"

#define MSG_HANGED_SUMMARY "Resumen"
#define MSG_HANGED_VICTORIES "Victorias"
#define MSG_HANGED_DEFEATS "Derrotas"

// Tipo server, permite inicializar y ejecutar un server
// para el juego ahorcado.
typedef struct {
    Socket socket;
    Hanged hanged;
    FileReader file_reader;
} Server;

// Tipo ServerState, permite conocer el estado del Server.
typedef enum{
    STATE_SUCCESS,
    STATE_ATTEMPTS_AMOUNT_ERROR,
    STATE_FILE_OPENING_ERROR,
    STATE_LISTENING_ERROR,
    STATE_CONNECTING_TO_CLIENT_ERROR,
    STATE_PACKING_INFO_ERROR,
    STATE_SENDING_PACKET_ERROR,
    STATE_RECEIVING_LETTER_ERROR,
    STATE_READING_WORD_ERROR
} ServerState;

/* Inicializa la instancia self, para ello toma un nombre
 * de archivo, un numero de puerto y la cantidad de intentos, todos
 * en forma de array de chars.
 * Pre: self debe apuntar a una direccion de memoria valida.
 *      filename debe apuntar a un array de chars con un nombre
 *      de archivo valido.
 *      port debe apuntar a un array de chars con un numero de puerto
 *      o un nombre de servicio valido.
 *      attempts debe apuntar a un array de cahrs con un numero de
 *      intentos valido.  */
ServerState serverInit(Server * self, char * filename,
                       char * attempts);

/* Ejecuta el server.
 * Pre: self debe apuntar a una direccion de memoria valida.  */
ServerState serverExecute(Server * self, char * port);

/* Desinicializa el server.
 * Pre: self debe apuntar a una direccion de memoria valida. */
void serverUnInit(Server * self);

/* Imprime el error correspondiente al state por stderr. */
void serverPrintError(ServerState state);


#endif //TP1_TALLER1_SERVER_H
