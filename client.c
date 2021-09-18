#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common_socket.h"
#include "common_hanged.h"


int main(int argc, char * argv[]){
    Socket socket;

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
    char buffer_letters[MAX_LETTERS_PER_SENT];
    HangedState state = STATE_IN_PROGRESS;
    unsigned short attempts;

    while(1){
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
        char c;
        int i = 0;
        memset(buffer_letters, 0, MAX_LETTERS_PER_SENT);
        for (i = 0; (c = getchar()) != '\n' && i < MAX_LETTERS_PER_SENT; i++){
            buffer_letters[i] = c;
        }
        if(c != '\n')
            while ((c = getchar()) != '\n' && c != EOF) {}
        if(socketSend(&socket, buffer_letters, MAX_LETTERS_PER_SENT) < 1){
            fprintf(stderr, "Error al enviar los datos de letra\n");
            return 1;
        }
    }

    if(state == STATE_PLAYER_WINS)
        printf("Ganaste!!\n");
    else
        printf("Perdiste! La palabra secreta era: %s\n", buffer_word);

    socketUnInit(&socket);

    return 0;







}


