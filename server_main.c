#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include "server.h"
#include "common_spanish.h"

#define MSG_ERROR_ARGS_AMOUNT "La cantidad de argumentos debe ser"
#define MSG_ERROR_INVALID_ATTEMPTS_AMOUNT "Cantidad de intentos invalida"
#define ARGUMENTS_SIZE 3

int main(int argc, char * argv[]){
    Server server;

    if(argc < (ARGUMENTS_SIZE + 1)){
        fprintf(stderr, "%s %d\n", MSG_ERROR_ARGS_AMOUNT, ARGUMENTS_SIZE);
        return 1;
    }

    long port_ = strtol(argv[1], NULL, 10);
    if (port_ <= 0 || port_ > USHRT_MAX) {
        fprintf(stderr, "%s\n", MSG_ERROR_INVALID_PORT_NUMBER);
        return EXIT_FAILURE;
    }
    uint16_t port = (uint16_t) port_;

    long attempts_ = strtol(argv[2], NULL, 10);
    if (attempts_ <= 0 || attempts_ > USHRT_MAX) {
        fprintf(stderr, "%s\n", MSG_ERROR_INVALID_ATTEMPTS_AMOUNT);
        return EXIT_FAILURE;
    }
    uint16_t attempts = (uint16_t) attempts_;

    serverInit(&server, argv[3], port, attempts);

    char buffer_error[MAX_ERROR_MSG_LENGTH];
    ServerState server_state;
    if((server_state = serverExecute(&server)) != STATE_SUCCESS) {
        serverGetError(server_state, buffer_error, MAX_ERROR_MSG_LENGTH);
        printf("%s\n", buffer_error);
        return EXIT_FAILURE;
    }

    printf("%s:\n", MSG_HANGED_SUMMARY);
    printf("\t%s: %d\n", MSG_HANGED_VICTORIES, serverGetPlayerVictories(&server));
    printf("\t%s: %d\n", MSG_HANGED_DEFEATS, serverGetPlayerDefeats(&server));

    serverUnInit(&server);

    return EXIT_SUCCESS;
}
