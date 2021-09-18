#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "socket.h"
#include "file_reader.h"
#include "hanged.h"
#include <errno.h>


int main(int argc, char * argv[]){
    FileReader file_reader;
    Socket socket, peer;
    Hanged hanged;

    int port = strtol(argv[1], NULL, 10);
    if (port <= 0) {
        fprintf(stderr, "Numero de puerto invalido\n");
        return 1;
    }

    int attempts = strtol(argv[2], NULL, 10);
    if (attempts <= 0 || attempts > MAX_ATTEMPTS) {
        fprintf(stderr, "Numero de intentos invalido\n");
        return 1;
    }

    if(fileReaderInitFromName(&file_reader, argv[3])){
        fprintf(stderr, "Error al abrir el archivo\n");
        return 1;
    }

    socketInit(&socket);
    if(socketBindAndListen(&socket, port)) {
        fprintf(stderr, "Error al poner el puerto en escucha\n");
        return 1;
    }


    char buffer[MAX_WORD_LENGTH + 1];
    char buffer_letters[MAX_LETTERS_PER_SENT];
    char package[MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE];
    while(!fileReaderEOF(&file_reader)){
        fileReaderReadLine(&file_reader, buffer, MAX_WORD_LENGTH + 1);
        if(buffer[0] == 0)
            continue;
        printf("%s\n", buffer);
        if(socketAccept(&socket, &peer)) {
            fprintf(stderr, "Error al conectar con el cliente\n");
            return 1;
        }
        if(hangedInit(&hanged, buffer, attempts)){
            fprintf(stderr, "Error al cargar palabra al juego\n");
            return 1;
        }

        while(hangedGetState(&hanged) == STATE_IN_PROGRESS) {
            int pack_size;
            memset(package, 0, MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE);
            if ((pack_size = hangedPackInformation(&hanged, package, MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE)) < 0) {
                fprintf(stderr, "Error al empaquetar informacion\n");
                return 1;
            }
            if (socketSend(&peer, package, pack_size) < 0) {
                fprintf(stderr, "Error al enviar paquete\n");
                return 1;
            }
            if (socketReceive(&peer, buffer_letters, MAX_LETTERS_PER_SENT) < 0) {
                fprintf(stderr, "Error al recibir letras\n");
                return 1;
            }
            for (int i = 0; i < MAX_LETTERS_PER_SENT && buffer_letters[i]; i++) {
                if(hangedTryLetter(&hanged, buffer_letters[i])){
                    fprintf(stderr, "Letra invalida\n");
                }
                printf("%ld\n", hangedGetAttempts(&hanged));
            }
        }
        int pack_size;
        memset(package, 0, MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE);
        if ((pack_size = hangedPackInformation(&hanged, package, MAX_WORD_LENGTH + INFORMATION_PACK_HEADER_SIZE)) < 0) {
            fprintf(stderr, "Error al empaquetar informacion\n");
            return 1;
        }
        if (socketSend(&peer, package, pack_size) < 0) {
            fprintf(stderr, "Error al enviar paquete\n");
            return 1;
        }
    }


    socketUnInit(&socket);
    socketUnInit(&peer);
    fileReaderUnInit(&file_reader);


    return 0;







}

