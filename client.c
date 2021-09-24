#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "common_socket.h"
#include "common_hanged.h"
#include "common_spanish.h"

#define ARGUMENTS_AMOUNT 2
#define MAX_LETTERS_PER_LINE 50


int main(int argc, char * argv[]){
    Socket socket;
    FileReader file_reader;

    fileReaderInit(&file_reader, stdin);

    if(argc < (ARGUMENTS_AMOUNT + 1)){
        fprintf(stderr, "%s %d\n", MSG_ERROR_ARGS_AMOUNT, ARGUMENTS_AMOUNT);
        return 1;
    }

    long port_ = strtol(argv[2], NULL, 10);
    if (port_ <= 0 || port_ > USHRT_MAX) {
        fprintf(stderr, "%s\n", MSG_ERROR_INVALID_PORT_NUMBER);
        return 1;
    }
    uint16_t port = (uint16_t) port_;

    socketInit(&socket);
    if(socketConnect(&socket, argv[1], port)) {
        fprintf(stderr, "%s\n", MSG_ERROR_SERVER_CONNECTION);
        return 1;
    }


    char package[MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE];


    char buffer_word[MAX_WORD_LENGTH];
    char buffer_letters[MAX_LETTERS_PER_LINE + 1];
    size_t read;
    HangedState state = STATE_IN_PROGRESS;
    unsigned short attempts;

    //Leo la primera vez lo que me mandan
    memset(package, 0, MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE);
    if(socketReceive(&socket, package, INFORMATION_PACK_HEADER_SIZE) < 0){
        fprintf(stderr, "%s\n", MSG_ERROR_PACKAGE_RECEIVE);
        return 1;
    }
    unsigned short size_word = hangedUnpackInformationHeader(package, &state, &attempts);

    if(socketReceive(&socket, package, size_word) < 0){
        fprintf(stderr, "%s\n", MSG_ERROR_PACKAGE_RECEIVE);
        return 1;
    }
    hangedUnpackInformationWord(package, buffer_word, size_word);


    printf("%s: %s\n", MSG_HANGED_SECRET_WORD, buffer_word);
    printf("%s %d %s\n", MSG_HANGED_ATTEMPTS_PARTONE, attempts, MSG_HANGED_ATTEMPTS_PARTTWO);
    printf("%s: ", MSG_HANGED_NEW_LETTER);

    while(1){
        int i = 0;

        if((read = fileReaderReadLine(&file_reader, buffer_letters, MAX_LETTERS_PER_LINE)) == -1){
            fprintf(stderr, "%s\n", MSG_ERROR_READING_LETTERS);
            return 1;
        }



        for (i = 0; i < read; i++){
            printf("\n");
            //Envio la letra
            if(buffer_letters[i] < 'a' || buffer_letters[i] > 'z')
				fprintf(stderr, "%s\n", MSG_ERROR_INVALID_LETTER);

			if(socketSend(&socket, &buffer_letters[i], 1) == -1) {
				fprintf(stderr, "%s\n", MSG_ERROR_SENDING_LETTERS);
				return 1;
			}


            memset(package, 0, MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE);
            if(socketReceive(&socket, package, INFORMATION_PACK_HEADER_SIZE) == -1){
				fprintf(stderr, "%s\n", MSG_ERROR_PACKAGE_RECEIVE);
                return 1;
            }
            unsigned short size_word = hangedUnpackInformationHeader(package, &state, &attempts);

            if(socketReceive(&socket, package, size_word) == -1){
                fprintf(stderr, "%s\n", MSG_ERROR_PACKAGE_RECEIVE);
                return 1;
            }
            hangedUnpackInformationWord(package, buffer_word, size_word);

            if(state != STATE_IN_PROGRESS)
                break;

			printf("%s: %s\n", MSG_HANGED_SECRET_WORD, buffer_word);
			printf("%s %d %s\n", MSG_HANGED_ATTEMPTS_PARTONE, attempts, MSG_HANGED_ATTEMPTS_PARTTWO);
			printf("%s: ", MSG_HANGED_NEW_LETTER);


        }

        if(state != STATE_IN_PROGRESS)
            break;

    }

    if(state == STATE_PLAYER_WINS)
        printf("%s!!\n", MSG_HANGED_YOU_WIN);
    else
        printf("%s: '%s'\n", MSG_HANGED_YOU_LOSE, buffer_word);
        

    socketUnInit(&socket);

    fileReaderUnInit(&file_reader);

    return 0;







}


