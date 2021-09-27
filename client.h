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

// Tipo Client, permite inicializar y ejecutar un cliente
// del juego ahorcado.
typedef struct {
    Socket socket;
    FileReader file_reader;
} Client;

// Tipo ClientState, permite conocer el estado del Client
typedef enum{
    STATE_SUCCESS,
    STATE_CONNECTION_ERROR,
    STATE_RECEIVING_PACKET_ERROR,
    STATE_READING_STDIN_ERROR,
    STATE_SENDING_LETTER_ERROR
} ClientState;

/* Inicializa la instancia self,.
 * Pre: self debe apuntar a una direccion de memoria valida. */
void clientInit(Client * self);

/* Conecta el cliente a un server.
 * Pre: self debe apuntar a unad direccion de memoria valida.
 *      host debe apuntar a un array de chars con un host valido.
 *      port debe apuntar a un array de chars con un numero de puerto
 *      o un nombre de servicio valido.  */
ClientState clientConnect(Client * self, char * host, char * port);

/* Ejecuta el cliente.
 * Pre: self debe apuntar a una direccion de memoria valida.  */
ClientState clientExecute(Client * self);

/* Desinicializa el cliente.
 * Pre: self debe apuntar a una direccion de memoria valida. */
void clientUnInit(Client * self);

/* Imprime el error correspondiente al state por stderr. */
void clientPrintError(ClientState state);

#endif //TP1_TALLER1_CLIENT_H
