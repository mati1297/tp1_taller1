#include <stdio.h>
#include "common_socket.h"
#include "common_hanged.h"
#include "client.h"

// Desempaca la informacion en el header del paquete recibido
// desde el server. Se obtiene asi el estado del juego, los
// intentos y el largo de la palabra.
static uint16_t _clientUnpackInformationHeader(char * packet,
                                               HangedState * state,
                                               uint8_t * attempts){
    // Se guarda el numero de intentos.
    *attempts = (uint8_t) packet[0] & MASK_ATTEMPTS;
    // Se guarda el bit del estado.
    uint8_t _state = (uint8_t) (packet[0] & MASK_STATE);
    // Se cruzan los datos de la cantidad de intentos y
    // de estado. Si el estado es 0, quiere decir que esta
    // en progreso. Pero si es 1, depende de la cantidad
    // de intentos restantes se decide si gana o pierde
    // el jugador.
    if (_state){
        if (*attempts > 0)
            *state = STATE_PLAYER_WINS;
        else
            *state = STATE_PLAYER_LOSES;
    } else {
        *state = STATE_IN_PROGRESS;
    }
    // Se obtiene el largo de la palabra y se pasa al
    // endianness del host.
    uint16_t string_length = 0;
    memcpy((char *) &string_length, &packet[1], sizeof(uint16_t));
    string_length = ntohs(string_length);
    return string_length;
}

/*  Se recibe la informacion enviada por el server y se desempaca, devolviendo
 *  directamente los parametros del juego y la palabra conocida hasta el
 *  momento */
static uint8_t _clientReceiveAndUnpackPacket(Client * self, HangedState * state,
                                             uint8_t * attempts,
                                             char ** buffer){
    // Se recibe y decodifica el header del paquete recibido.
    char packet_header[INFORMATION_PACK_HEADER_SIZE];
    memset(packet_header, 0, INFORMATION_PACK_HEADER_SIZE);

    if (socketReceive(&self->socket,
                      packet_header, INFORMATION_PACK_HEADER_SIZE)
                      != INFORMATION_PACK_HEADER_SIZE)
        return 1;

    uint16_t size_word = _clientUnpackInformationHeader(packet_header, state,
                                                        attempts);

    // Una vez que se tiene el tamanio de la palabra, se alloca
    // el buffer y se recibe la palabra.
    if (!(*buffer = malloc(size_word + 1))) {
        return 1;
    }

    memset(*buffer, 0, size_word + 1);

    if (socketReceive(&self->socket, *buffer, size_word) != size_word) {
        free(*buffer);
        *buffer = NULL;
        return 1;
    }

    return 0;
}

/* Imprime el mensaje de progreso del cliente */
static void _clientPrintProgressMessage(uint8_t attempts, char * buffer){
    printf("%s: %s\n", MSG_HANGED_SECRET_WORD, buffer);
    printf("%s %d %s\n", MSG_HANGED_ATTEMPTS_PARTONE, attempts,
           MSG_HANGED_ATTEMPTS_PARTTWO);
    printf("%s: \n", MSG_HANGED_NEW_LETTER);
}

/* Imprime el mensaje final del cliente, donde se dice
 * si el cliente gano o no, y muestra la palabra correcta de
 * ser necesario */
static void _clientPrintFinalMessage(HangedState game_state, char * buffer){
    if (game_state == STATE_PLAYER_WINS)
        printf("%s\n", MSG_HANGED_YOU_WIN);
    else
        printf("%s: '%s'\n", MSG_HANGED_YOU_LOSE, buffer);
}

void clientInit(Client * self){
    socketInit(&self->socket);
    fileReaderInit(&self->file_reader, stdin);
}

ClientState clientConnect(Client * self, char * host, char * port){
    if (socketConnect(&self->socket, host, port))
        return STATE_CONNECTION_ERROR;
    return STATE_SUCCESS;
}

ClientState clientExecute(Client * self){
    char * buffer_word;
    HangedState game_state = STATE_IN_PROGRESS;
    uint8_t attempts;

    // Se lee e imprime el primer paquete
    if (_clientReceiveAndUnpackPacket(self, &game_state,
                                      &attempts, &buffer_word))
        return STATE_RECEIVING_PACKET_ERROR;

    _clientPrintProgressMessage(attempts, buffer_word);

    // Se libera el buffer.
    free(buffer_word);
    buffer_word = NULL;

    char * buffer_letters = NULL;
    size_t buffer_letters_size = 0;


    // Se itera mientras que el juego este en progreso.
    while (game_state == STATE_IN_PROGRESS){
        size_t read;

        // Se lee una linea de standard in.
        if ((read = fileReaderReadLine(&self->file_reader, &buffer_letters,
                                       &buffer_letters_size)) == -1) {
            free(buffer_letters);
            buffer_letters = NULL;
            return STATE_READING_STDIN_ERROR;
        }

        // Se itera por cada letra ingresada por stdin.
        for (int i = 0; i < read; i++){
            // Se libera al principio si es que esta allocado ya
            // que cuando termina el ciclo no debe ser liberado
            // para poder imprimir el mensaje final.
            if (buffer_word){
                free(buffer_word);
                buffer_word = NULL;
            }

            // Se verifica que la letra sea valida, de lo contrario
            // se ignora y se imprime un mensaje por stdout.
            if (buffer_letters[i] < 'a' || buffer_letters[i] > 'z') {
                printf("%s\n", MSG_ERROR_INVALID_LETTER);
                continue;
            }

            // Se envia la letra.
            if (socketSend(&self->socket, &buffer_letters[i], 1) == -1) {
                free(buffer_letters);
                buffer_letters = NULL;
                return STATE_SENDING_LETTER_ERROR;
            }

            // Se recibe un paquete y se desempaca.
            if (_clientReceiveAndUnpackPacket(self, &game_state,
                                              &attempts, &buffer_word)) {
                free(buffer_word);
                buffer_letters = NULL;
                return STATE_RECEIVING_PACKET_ERROR;
            }

            // Si el juego sigue en proceso se imprime el mensaje pidiendo
            // otra letra.
            if (game_state == STATE_IN_PROGRESS)
                _clientPrintProgressMessage(attempts, buffer_word);
            else
                break;
        }

        // Se libera la memoria de la lectura de stdin.
        free(buffer_letters);
        buffer_letters = NULL;
    }
    // Se imprime el mensaje final.
    _clientPrintFinalMessage(game_state, buffer_word);

    // Se libera la memoria del buffer de palabras.
    free(buffer_word);
    buffer_word = NULL;
    return STATE_SUCCESS;
}

void clientUnInit(Client * self){
    socketUnInit(&self->socket);
    fileReaderUnInit(&self->file_reader);
}

void clientPrintError(ClientState state){
    switch (state){
        case STATE_CONNECTION_ERROR:
            fprintf(stderr, "%s\n", MSG_ERROR_SERVER_CONNECTION);
            break;
        case STATE_RECEIVING_PACKET_ERROR:
            fprintf(stderr, "%s\n", MSG_ERROR_PACKAGE_RECEIVE);
            break;
        case STATE_READING_STDIN_ERROR:
            fprintf(stderr, "%s\n", MSG_ERROR_READING_LETTERS);
            break;
        case STATE_SENDING_LETTER_ERROR:
            fprintf(stderr, "%s\n", MSG_ERROR_SENDING_LETTERS);
        default:
            break;
    }
}

