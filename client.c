#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common_socket.h"
#include "common_hanged.h"

#define ARGUMENTS_SIZE 2

int main(int argc, char * argv[]){
    Socket socket;
    FileReader file_reader;

    fileReaderInit(&file_reader, stdin);

    if(argc < (ARGUMENTS_SIZE + 1)){
        fprintf(stderr, "La cantidad de argumentos debe ser %d\n", ARGUMENTS_SIZE);
        return 1;
    }

    int port = strtol(argv[2], NULL, 10);
    if (port <= 0) {
        fprintf(stderr, "Numero de puerto invalido\n");
        return 1;
    }

    socketInit(&socket);
    if(socketConnect(&socket, argv[1], port)) {
        fprintf(stderr, "Error al conectar al server\n");
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
        fprintf(stderr, "Error al recibir el paquete de información\n");
        return 1;
    }
    unsigned short size_word = hangedUnpackInformationHeader(package, &state, &attempts);

    if(socketReceive(&socket, package, size_word) < 0){
        fprintf(stderr, "Error al recibir el paquete de información\n");
        return 1;
    }
    hangedUnpackInformationWord(package, buffer_word, size_word);


    printf("Palabra secreta: %s\n", buffer_word);
    printf("Te quedan %d intentos\n", attempts);
    printf("Ingrese letra: ");

    while(1){
        int i = 0;

        if((read = fileReaderReadLine(&file_reader, buffer_letters, MAX_LETTERS_PER_LINE)) == -1){
            fprintf(stderr, "Error al leer las letras de entrada");
            return 1;
        }

        for (i = 0; i < read; i++){
            printf("\n");
            //Envio la letra
            if(socketSend(&socket, &buffer_letters[i], 1) < 1) {
                fprintf(stderr, "Error al enviar los datos de letra\n");
                return 1;
            }

            memset(package, 0, MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE);
            if(socketReceive(&socket, package, INFORMATION_PACK_HEADER_SIZE) < 0){
                fprintf(stderr, "Error al recibir el paquete de información\n");
                return 1;
            }
            unsigned short size_word = hangedUnpackInformationHeader(package, &state, &attempts);

            if(socketReceive(&socket, package, size_word) < 0){
                fprintf(stderr, "Error al recibir el paquete de información\n");
                return 1;
            }
            hangedUnpackInformationWord(package, buffer_word, size_word);

            if(state != STATE_IN_PROGRESS)
                break;

            printf("Palabra secreta: %s\n", buffer_word);
            printf("Te quedan %d intentos\n", attempts);
            printf("Ingrese letra: ");


        }

        if(state != STATE_IN_PROGRESS)
            break;

    }

    if(state == STATE_PLAYER_WINS)
        printf("Ganaste!!\n");
    else
        printf("Perdiste! La palabra secreta era: '%s'\n", buffer_word);

    socketUnInit(&socket);

    fileReaderUnInit(&file_reader);

    return 0;







}


