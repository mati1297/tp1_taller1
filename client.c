#include <stdio.h>
#include "common_socket.h"
#include "common_hanged.h"
#include "client.h"

static uint8_t _clientReceiveAndUnpackPacket(Client * self, HangedState * state,
                                             uint8_t * attempts, char * buffer){
    char packet[MAX_WORD_LENGTH  + INFORMATION_PACK_HEADER_SIZE];
    memset(packet, 0, MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE);

    if (socketReceive(&self->socket, packet, INFORMATION_PACK_HEADER_SIZE) < 0)
        return 1;

    uint16_t size_word = hangedUnpackInformationHeader(packet, state, attempts);

    if (socketReceive(&self->socket, packet, size_word) < 0)
        return 1;

    hangedUnpackInformationWord(packet, buffer, size_word);
    return 0;
}

static void _clientPrintProgressMessage(uint8_t attempts, char * buffer){
    printf("%s: %s\n", MSG_HANGED_SECRET_WORD, buffer);
    printf("%s %d %s\n", MSG_HANGED_ATTEMPTS_PARTONE, attempts,
           MSG_HANGED_ATTEMPTS_PARTTWO);
    printf("%s: \n", MSG_HANGED_NEW_LETTER);
}

static void _clientPrintFinalMessage(HangedState game_state, char * buffer){
    if (game_state == STATE_PLAYER_WINS)
        printf("%s!!\n", MSG_HANGED_YOU_WIN);
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
    char buffer_word[MAX_WORD_LENGTH];
    HangedState game_state = STATE_IN_PROGRESS;
    uint8_t attempts;

    // Se lee e imprime el primer paquete
    if (_clientReceiveAndUnpackPacket(self, &game_state,
                                      &attempts, buffer_word))
        return STATE_RECEIVING_PACKET_ERROR;

    _clientPrintProgressMessage(attempts, buffer_word);

    char * buffer_letters = NULL;
    size_t buffer_letters_size = 0;

    while (game_state == STATE_IN_PROGRESS){
        size_t read;
        if ((read = fileReaderReadLine(&self->file_reader, &buffer_letters,
                                       &buffer_letters_size)) == -1) {
            free(buffer_letters);
            buffer_letters = NULL;
            return STATE_READING_STDIN_ERROR;
        }

        for (int i = 0; i < read; i++){
            //Envio la letra
            if (buffer_letters[i] < 'a' || buffer_letters[i] > 'z') {
                printf("%s", MSG_ERROR_INVALID_LETTER);
                continue;
            }

            if (socketSend(&self->socket, &buffer_letters[i], 1) == -1) {
                free(buffer_letters);
                buffer_letters = NULL;
                return STATE_SENDING_LETTER_ERROR;
            }

            if (_clientReceiveAndUnpackPacket(self, &game_state,
                                              &attempts, buffer_word)) {
                free(buffer_letters);
                buffer_letters = NULL;
                return STATE_RECEIVING_PACKET_ERROR;
            }

            if (game_state == STATE_IN_PROGRESS)
                _clientPrintProgressMessage(attempts, buffer_word);
            else
                break;
        }
    }
    free(buffer_letters);
    buffer_letters = NULL;
    _clientPrintFinalMessage(game_state, buffer_word);

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

