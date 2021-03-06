#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include "server.h"

#define MSG_ERROR_ARGS_AMOUNT "La cantidad de argumentos debe ser"
#define ARGUMENTS_SIZE 3

int main(int argc, char * argv[]){
    if (argc < (ARGUMENTS_SIZE + 1)){
        fprintf(stderr, "%s %d\n", MSG_ERROR_ARGS_AMOUNT, ARGUMENTS_SIZE);
        return 1;
    }
    Server server;
    serverInit(&server, argv[3], argv[2]);

    ServerState server_state;
    if ((server_state = serverExecute(&server, argv[1])) != STATE_SUCCESS) {
        serverPrintError(server_state);
        serverUnInit(&server);
        return EXIT_FAILURE;
    }

    serverUnInit(&server);

    return EXIT_SUCCESS;
}
