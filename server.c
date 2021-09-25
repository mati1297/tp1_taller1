#include <stdio.h>
#include "common_socket.h"
#include "common_file_reader.h"
#include "common_hanged.h"
#include "server.h"

void _serverPrintFinalMessage(Server * self){
    printf("%s:\n", MSG_HANGED_SUMMARY);
    printf("\t%s: %ld\n", MSG_HANGED_VICTORIES, hangedGetVictories(&self->hanged));
    printf("\t%s: %ld\n", MSG_HANGED_DEFEATS, hangedGetDefeats(&self->hanged));
}

ServerState serverInit(Server * self, char * filename, char * port, char * attempts){
    if(fileReaderInitFromName(&self->file_reader, filename))
        return STATE_FILE_OPENING_ERROR;

    long attempts_ = strtol(attempts, NULL, 10);
    if (attempts_ <= 0 || attempts_ > MAX_ATTEMPTS) {
        fprintf(stderr, "%s\n", MSG_ERROR_INVALID_ATTEMPTS_AMOUNT);
        return EXIT_FAILURE;
    }
    uint8_t attempts_number = (uint8_t) attempts_;

    socketInit(&self->socket);
    if(socketBindAndListen(&self->socket, port))
        return STATE_LISTENING_ERROR;

    hangedInit(&self->hanged, attempts_number);

    return STATE_SUCCESS;
}

ServerState serverExecute(Server * self){
    char buffer[MAX_WORD_LENGTH + 1];
    char package[MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE];
    char new_letter;
    int packet_size;

    while(!fileReaderEOF(&self->file_reader)){
        fileReaderReadLine(&self->file_reader, buffer, MAX_WORD_LENGTH + 1);
        if(buffer[0] == 0)
            continue;
        if(hangedAddWord(&self->hanged, buffer))
            continue;

        if(socketAccept(&self->socket, &self->peer))
            return STATE_CONNECTING_TO_CLIENT_ERROR;

        while(hangedGetState(&self->hanged) == STATE_IN_PROGRESS) {

            memset(package, 0, MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE);
            if ((packet_size = hangedPackInformation(&self->hanged, package, MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE)) == -1)
                return STATE_PACKING_INFO_ERROR;

            if (socketSend(&self->peer, package, packet_size) == -1)
                return STATE_SENDING_PACKET_ERROR;

            if (socketReceive(&self->peer, &new_letter, 1) == -1)
                return STATE_RECEIVING_LETTER_ERROR;

            hangedTryLetter(&self->hanged, new_letter);
        }

        memset(package, 0, MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE);
        if ((packet_size = hangedPackInformation(&self->hanged, package, MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE)) < 0)
            return STATE_PACKING_INFO_ERROR;

        if (socketSend(&self->peer, package, packet_size) == -1)
            return STATE_SENDING_PACKET_ERROR;
    }

    _serverPrintFinalMessage(self);
    return STATE_SUCCESS;
}

void serverUnInit(Server * self){
    socketUnInit(&self->socket);
    socketUnInit(&self->peer);
    fileReaderUnInit(&self->file_reader);
}

void serverPrintError(ServerState state){
    switch(state){
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
        default:
            break;
    }
}


