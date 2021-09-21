#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "common_socket.h"
#include "common_file_reader.h"
#include "common_hanged.h"
#include "common_spanish.h"

#define ARGUMENTS_SIZE 3

int main(int argc, char * argv[]){
    FileReader file_reader;
    Socket socket, peer;
    Hanged hanged;

    if(argc < (ARGUMENTS_SIZE + 1)){
        fprintf(stderr, "%s %d\n", MSG_ERROR_ARGS_AMOUNT, ARGUMENTS_SIZE);
        return 1;
    }

    int port = strtol(argv[1], NULL, 10);
    if (port <= 0) {
        fprintf(stderr, "%s\n", MSG_ERROR_INVALID_LETTER);
        return 1;
    }

    int attempts = strtol(argv[2], NULL, 10);
    if (attempts <= 0 || attempts > MAX_ATTEMPTS) {
        fprintf(stderr, "%s\n", MSG_ERROR_INVALID_ATTEMPTS_AMOUNT);
        return 1;
    }

    if(fileReaderInitFromName(&file_reader, argv[3])){
        fprintf(stderr, "%s\n", MSG_ERROR_OPEN_FILE);
        return 1;
    }

    socketInit(&socket);
    if(socketBindAndListen(&socket, port)) {
        fprintf(stderr, "%s\n", MSG_ERROR_LISTENING_PORT);
        return 1;
    }


    char buffer[MAX_WORD_LENGTH + 1];
    char package[MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE];
    char new_letter;

    hangedInit(&hanged, attempts);
    while(!fileReaderEOF(&file_reader)){
        fileReaderReadLine(&file_reader, buffer, MAX_WORD_LENGTH + 1);
        if(buffer[0] == 0)
            continue;
        if(hangedAddWord(&hanged, buffer)){
            fprintf(stderr, "%s\n", MSG_ERROR_LOADING_WORD);
			continue;
        }    
        if(socketAccept(&socket, &peer)) {
            fprintf(stderr, "%s\n", MSG_ERROR_CONNECTING_CLIENT);
            return 1;
        }
        

        while(hangedGetState(&hanged) == STATE_IN_PROGRESS) {
            int pack_size;
            memset(package, 0, MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE);
            if ((pack_size = hangedPackInformation(&hanged, package, MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE)) < 0) {
                fprintf(stderr, "%s\n", MSG_ERROR_PACKING_INFO);
                return 1;
            }
            if (socketSend(&peer, package, pack_size) < 0) {
                fprintf(stderr, "%s\n", MSG_ERROR_SENDING_PACKAGE);
                return 1;
            }
            if (socketReceive(&peer, &new_letter, 1) < 0) {
                fprintf(stderr, "%s\n", MSG_ERROR_LETTERS_RECEIVE);
                return 1;
            }
            if(hangedTryLetter(&hanged, new_letter))
                fprintf(stderr, "%s\n", MSG_ERROR_INVALID_LETTER);
        }
        int pack_size;
        memset(package, 0, MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE);
        if ((pack_size = hangedPackInformation(&hanged, package, MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE)) < 0) {
            fprintf(stderr, "%s\n", MSG_ERROR_PACKING_INFO);
            return 1;
        }
        if (socketSend(&peer, package, pack_size) < 0) {
            fprintf(stderr, "%s\n", MSG_ERROR_SENDING_PACKAGE);
            return 1;
        }
    }

    printf("%s:\n", MSG_HANGED_SUMMARY);
    printf("\t%s: %ld\n", MSG_HANGED_VICTORIES, hangedGetVictories(&hanged));
    printf("\t%s: %ld\n", MSG_HANGED_DEFEATS, hangedGetLoses(&hanged));


    socketUnInit(&socket);
    socketUnInit(&peer);
    fileReaderUnInit(&file_reader);


    return 0;







}

