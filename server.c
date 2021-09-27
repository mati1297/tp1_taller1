#include <stdio.h>
#include "common_socket.h"
#include "common_file_reader.h"
#include "common_hanged.h"
#include "server.h"

static ssize_t _serverPackInformation(Hanged * hanged, char ** packet) {
    const char * known_word = hangedGetKnownWord(hanged);
    uint16_t word_size = strlen(known_word);
    size_t required_size = word_size + INFORMATION_PACK_HEADER_SIZE;
    if (!(*packet = malloc(required_size + 1)))
        return -1;
    (*packet)[0] = (char) (hangedGetAttemptsCount(hanged)) & MASK_ATTEMPTS;
    if (hangedGetState(hanged) == STATE_IN_PROGRESS)
        (*packet)[0] &= MASK_STATE_IN_PROGRESS;
    else
        (*packet)[0] |= MASK_STATE_FINISHED;
    uint16_t word_size_big_endian = htons(word_size);
    memcpy(&(*packet)[1], (const char *) &word_size_big_endian,
           sizeof(uint16_t));
    strncpy(&(*packet)[3], known_word, word_size + 1);
    return INFORMATION_PACK_HEADER_SIZE + word_size;
}

void _serverPrintFinalMessage(Server * self) {
    printf("%s:\n", MSG_HANGED_SUMMARY);
    printf("\t%s: %zu\n", MSG_HANGED_VICTORIES,
           hangedGetVictories(&self->hanged));
    printf("\t%s: %zu\n", MSG_HANGED_DEFEATS,
           hangedGetDefeats(&self->hanged));
}

ServerState serverInit(Server * self, char * filename,
                       char * port, char * attempts){
    if (fileReaderInitFromName(&self->file_reader, filename))
        return STATE_FILE_OPENING_ERROR;

    long attempts_ = strtol(attempts, NULL, 10);
    if (attempts_ <= 0 || attempts_ > MAX_ATTEMPTS) {
        fprintf(stderr, "%s\n", MSG_ERROR_INVALID_ATTEMPTS_AMOUNT);
        return EXIT_FAILURE;
    }
    uint8_t attempts_number = (uint8_t) attempts_;

    socketInit(&self->socket);
    if (socketBindAndListen(&self->socket, port))
        return STATE_LISTENING_ERROR;

    hangedInit(&self->hanged, attempts_number);

    return STATE_SUCCESS;
}

ServerState serverExecute(Server * self){
    char * buffer = NULL;
    size_t buffer_size = 0;
    char * packet;
    char new_letter;
    int packet_size;


    while (!fileReaderEOF(&self->file_reader)){
        ssize_t read = fileReaderReadLine(&self->file_reader,
                                          &buffer, &buffer_size);
        if (read == -1) {
            return STATE_READING_WORD_ERROR;
        }
        if (read == 0){
            continue;
        }
        if (read > MAX_WORD_LENGTH) {
            free(buffer);
            buffer = NULL;
            continue;
        }
        if (buffer[0] == 0){
            free(buffer);
            buffer = NULL;
            continue;
        }
        if (hangedAddWord(&self->hanged, buffer)) {
            free(buffer);
            buffer = NULL;
            continue;
        }
        free(buffer);
        buffer = NULL;

        if (socketAccept(&self->socket, &self->peer)) {
            return STATE_CONNECTING_TO_CLIENT_ERROR;
        }

        while (hangedGetState(&self->hanged) == STATE_IN_PROGRESS) {
            if ((packet_size = _serverPackInformation(
                    &self->hanged, &packet)) == -1) {
                free(packet);
                return STATE_PACKING_INFO_ERROR;
            }
            if (socketSend(&self->peer, packet, packet_size) == -1){
                free(packet);
                return STATE_SENDING_PACKET_ERROR;
            }
            free(packet);
            if (socketReceive(&self->peer, &new_letter, 1) == -1)
                return STATE_RECEIVING_LETTER_ERROR;
            hangedTryLetter(&self->hanged, new_letter);
        }

        if ((packet_size = _serverPackInformation(
                &self->hanged, &packet)) < 0) {
            free(packet);
            return STATE_PACKING_INFO_ERROR;
        }

        if (socketSend(&self->peer, packet, packet_size) == -1) {
            free(packet);
            return STATE_SENDING_PACKET_ERROR;
        }
        free(packet);

        socketUnInit(&self->peer);
    }

    _serverPrintFinalMessage(self);
    return STATE_SUCCESS;
}

void serverUnInit(Server * self){
    socketUnInit(&self->socket);
    socketUnInit(&self->peer);
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


