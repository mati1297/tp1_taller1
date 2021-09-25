#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "common_socket.h"
#include "common_hanged.h"
#include "common_spanish.h"
#include "client_class.h"

#define ARGUMENTS_AMOUNT 2



int main(int argc, char * argv[]) {
    /*
    Socket socket;
    FileReader file_reader;

    fileReaderInit(&file_reader, stdin);
*/
    if (argc < (ARGUMENTS_AMOUNT + 1)) {
        fprintf(stderr, "%s %d\n", MSG_ERROR_ARGS_AMOUNT, ARGUMENTS_AMOUNT);
        return 1;
    }

    Client client;
    ClientState client_state;

    clientInit(&client);


    if ((client_state = clientConnect(&client, argv[1], argv[2])) != STATE_SUCCESS){
        clientPrintError(client_state);
        clientUnInit(&client);
        return EXIT_FAILURE;
    }

    if ((client_state = clientExecute(&client)) != STATE_SUCCESS){
        clientPrintError(client_state);
        clientUnInit(&client);
        return EXIT_FAILURE;
    }

    clientUnInit(&client);
    return EXIT_SUCCESS;
}



