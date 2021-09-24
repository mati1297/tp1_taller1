#include <stdio.h>
#include "common_socket.h"
#include "common_file_reader.h"
#include "common_hanged.h"
#include "server.h"

ServerState serverInit(Server * self, char * filename, uint16_t port, uint16_t attempts){
    if(fileReaderInitFromName(&self->file_reader, filename))
        return STATE_FILE_OPENING_ERROR;

    socketInit(&self->socket);
    if(socketBindAndListen(&self->socket, port))
        return STATE_LISTENING_ERROR;

    hangedInit(&self->hanged, attempts);

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

    return STATE_SUCCESS;
}

HangedState serverGetPlayerVictories(Server * self){
    return hangedGetVictories(&self->hanged);
}

HangedState serverGetPlayerDefeats(Server * self){
    return hangedGetDefeats(&self->hanged);
}

void serverUnInit(Server * self){
    socketUnInit(&self->socket);
    socketUnInit(&self->peer);
    fileReaderUnInit(&self->file_reader);
}

void serverGetError(ServerState state, char * buffer, size_t size){
    switch(state){
        case STATE_FILE_OPENING_ERROR:
            strncpy(buffer, MSG_ERROR_OPEN_FILE, size);
            break;
        case STATE_LISTENING_ERROR:
            strncpy(buffer, MSG_ERROR_LISTENING_PORT, size);
            break;
        case STATE_CONNECTING_TO_CLIENT_ERROR:
            strncpy(buffer, MSG_ERROR_CONNECTING_CLIENT, size);
            break;
        case STATE_PACKING_INFO_ERROR:
            strncpy(buffer, MSG_ERROR_PACKING_INFO, size);
            break;
        case STATE_SENDING_PACKET_ERROR:
            strncpy(buffer, MSG_ERROR_SENDING_PACKET, size);
            break;
        case STATE_RECEIVING_LETTER_ERROR:
            strncpy(buffer, MSG_ERROR_LETTERS_RECEIVE, size);
            break;
        default:
            break;
    }
}


