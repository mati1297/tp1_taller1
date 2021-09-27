#include <stdio.h>
#include "common_socket.h"
#include "common_file_reader.h"
#include "common_hanged.h"
#include "server.h"

/* Empaqueta la informacion del juego que debe ser enviada. Para
 * ello recibe un doble puntero a char, al cual se le asigna
 * posiciones de memoria donde se aloca el paquete preparado
 * para ser enviado. */
static ssize_t _serverPackInformation(Server * self, char ** packet) {
    // Se obtiene la palabra conocida hasta ahora.
    const char * known_word = hangedGetKnownWord(&self->hanged);
    uint16_t word_size = strlen(known_word);
    // Se calcula el tamanio que se necesita.
    size_t required_size = word_size + INFORMATION_PACK_HEADER_SIZE;
    // Se pide memoria.
    if (!(*packet = malloc(required_size + 1)))
        return -1;
    // Se guarda la cantidad de intentos (los ultimos 7 bits).
    (*packet)[0] = (char) (hangedGetAttemptsCount(&self->hanged))
                           & MASK_ATTEMPTS;
    // Se setea el primer bit del primer byte como 0 o 1 dependiendo
    // del estado del juego.
    if (hangedGetState(&self->hanged) == STATE_IN_PROGRESS)
        (*packet)[0] &= MASK_STATE_IN_PROGRESS;
    else
        (*packet)[0] |= MASK_STATE_FINISHED;
    // Se guarda el tamanio de la palabra en formato big endian.
    uint16_t word_size_big_endian = htons(word_size);
    // Se guardan el tamanio de la palabra y luego la palabra en
    // el el paquete.
    memcpy(&(*packet)[1], (const char *) &word_size_big_endian,
           sizeof(uint16_t));
    strncpy(&(*packet)[3], known_word, word_size + 1);
    return INFORMATION_PACK_HEADER_SIZE + word_size;
}

/* Empaqueta los datos que deben ser enviados al cliente utilizando
 * _serverPackInformation y luego la envia a traves del socket
 * peer. */
static uint8_t _serverSendPacket(Server * self, Socket * peer){
    char * packet;
    int packet_size;
    // Se empaqueta la informacion
    if ((packet_size = _serverPackInformation(
            self, &packet)) == -1) {
        free(packet);
        socketUnInit(peer);
        return 1;
    }
    // Se envia la informacion
    if (socketSend(peer, packet, packet_size) == -1){
        free(packet);
        return 1;
    }
    // Se libera la memoria usada por el paquete.
    free(packet);
    return 0;
}

/*  Realiza la lectura desde el file reader, valida la lectura
 * y la palabra, y la carga en el juego.*/
static uint8_t _serverReadAndAddWord(Server * self){
    char * buffer = NULL;
    size_t buffer_size = 0;

    // Se lee una linea
    ssize_t read = fileReaderReadLine(&self->file_reader,
                                      &buffer, &buffer_size);
    // Si la lectura fallo, se devuelve un estado de error.
    if (read == -1) {
        free(buffer);
        buffer = NULL;
        return 2;
    }
    // Si no se leyo nada en la linea, se sigue con la siguiente.
    if (read == 0){
        free(buffer);
        buffer = NULL;
        return 1;
    }
    // Si hay mas cantidad de caracteres que lo que tiene mayor tamanio
    // que lo que puede ser representado por 2 bytes, se sigue con la
    // siguiente linea.
    if (read > MAX_WORD_LENGTH) {
        free(buffer);
        buffer = NULL;
        return 1;
    }
    // Si se leyo una linea nula, se sigue con la siguiente.
    // (esto ocurre con una linea que solo tiene un '\n').
    if (buffer[0] == 0){
        free(buffer);
        buffer = NULL;
        return 1;
    }
    // Se agrega la palabra al juego.
    if (hangedAddWord(&self->hanged, buffer)) {
        free(buffer);
        buffer = NULL;
        return 1;
    }
    // Se libera el buffer.
    free(buffer);
    buffer = NULL;
    return 0;
}

// Imprime el mensaje final del server.
static void _serverPrintFinalMessage(Server * self) {
    printf("%s:\n", MSG_HANGED_SUMMARY);
    printf("\t%s: %zu\n", MSG_HANGED_VICTORIES,
           hangedGetVictories(&self->hanged));
    printf("\t%s: %zu\n", MSG_HANGED_DEFEATS,
           hangedGetDefeats(&self->hanged));
}

ServerState serverInit(Server * self, char * filename,
                       char * port, char * attempts){
    // Se inicializa el file reader con el nombre pasado
    // por parametro.
    if (fileReaderInitFromName(&self->file_reader, filename))
        return STATE_FILE_OPENING_ERROR;

    // Se convierte a numero y se valida la cantidad de intentos.
    long attempts_ = strtol(attempts, NULL, 10);
    if (attempts_ <= 0 || attempts_ > MAX_ATTEMPTS) {
        fprintf(stderr, "%s\n", MSG_ERROR_INVALID_ATTEMPTS_AMOUNT);
        return EXIT_FAILURE;
    }
    uint8_t attempts_number = (uint8_t) attempts_;

    // Se inicializa y se pone en escucha el socket.
    socketInit(&self->socket);
    if (socketBindAndListen(&self->socket, port))
        return STATE_LISTENING_ERROR;

    // Se inicializa el juego Hanged.
    hangedInit(&self->hanged, attempts_number);

    return STATE_SUCCESS;
}

ServerState serverExecute(Server * self){
    char new_letter;
    Socket peer;

    // Se itera a traves de las lineas del archivo de entrada.
    while (!fileReaderEOF(&self->file_reader)){
        // Se lee una linea y se agrega la palabra al juego
        uint8_t add_word_state;
        if ((add_word_state = _serverReadAndAddWord(self)) == 1)
            continue;
        else if (add_word_state == 2)
            return STATE_READING_WORD_ERROR;

        // Se acepta a un cliente que este en espera.
        if (socketAccept(&self->socket, &peer)) {
            return STATE_CONNECTING_TO_CLIENT_ERROR;
        }

        // Se itera mientras que el juego se mantenga en proceso.
        while (hangedGetState(&self->hanged) == STATE_IN_PROGRESS) {
            // Se crea y se envia al cliente el paquete de informacion.
            if (_serverSendPacket(self, &peer)){
                socketUnInit(&peer);
                return STATE_SENDING_PACKET_ERROR;
            }

            // Se reciben las letras enviadas por el cliente.
            if (socketReceive(&peer, &new_letter, 1) != 1) {
                socketUnInit(&peer);
                return STATE_RECEIVING_LETTER_ERROR;
            }
            // Se prueba la letra en el juego.
            hangedTryLetter(&self->hanged, new_letter);
        }
        // Una vez finalizado el juego, se envia un ultimo paquete
        // al cliente para que se entere de la situacion.
        if (_serverSendPacket(self, &peer)){
            socketUnInit(&peer);
            return STATE_SENDING_PACKET_ERROR;
        }

        // Se desinicializa el socket del cliente.
        socketUnInit(&peer);
    }

    // Se imprime el mensaje final con el conteo de victorias
    // y derrotas de los jugadores.
    _serverPrintFinalMessage(self);
    return STATE_SUCCESS;
}

void serverUnInit(Server * self){
    socketUnInit(&self->socket);
    hangedUnInit(&self->hanged);
    fileReaderUnInit(&self->file_reader);
}

void serverPrintError(ServerState state){
    switch (state){
        case STATE_FILE_OPENING_ERROR:
            fprintf(stderr, "%s\n", MSG_ERROR_OPEN_FILE);
            break;
        case STATE_ATTEMPTS_AMOUNT_ERROR:
            fprintf(stderr, "%s\n", MSG_ERROR_INVALID_ATTEMPTS_AMOUNT);
            break;
        case STATE_LISTENING_ERROR:
            fprintf(stderr, "%s\n", MSG_ERROR_LISTENING_PORT);
            break;
        case STATE_CONNECTING_TO_CLIENT_ERROR:
            fprintf(stderr, "%s:\n", MSG_ERROR_CONNECTING_CLIENT);
            perror("");
            break;
        case STATE_PACKING_INFO_ERROR:
            fprintf(stderr, "%s\n", MSG_ERROR_PACKING_INFO);
            break;
        case STATE_SENDING_PACKET_ERROR:
            fprintf(stderr, "%s\n", MSG_ERROR_SENDING_PACKET);
            break;
        case STATE_RECEIVING_LETTER_ERROR:
            fprintf(stderr, "%s\n", MSG_ERROR_LETTERS_RECEIVE);
            break;
        case STATE_READING_WORD_ERROR:
            fprintf(stderr, "%s\n", MSG_ERROR_READING_WORD);
            break;
        default:
            break;
    }
}


