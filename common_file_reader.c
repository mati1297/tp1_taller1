#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include "common_file_reader.h"

uint16_t fileReaderInit(FileReader * self, FILE * fds){
    if (!fds)
        return 1;
    self->fds = fds;
    return 0;
}

uint16_t fileReaderInitFromName(FileReader * self, char * name){
    if (!(self->fds = fopen(name, "r")))
        return 1;
    return 0;
}

void fileReaderUnInit(FileReader * self){
    if (self->fds && self->fds != stdin)
        fclose(self->fds);
}

bool fileReaderEOF(FileReader * self){
    return feof(self->fds);
}

ssize_t fileReaderReadLine(FileReader * self, char ** output, size_t * size){
    ssize_t read;
    // Se usa getline() para leer la linea de entrada.
    read = getline(output, size, self->fds);
    // En caso de fallar la getline se libera la memoria  y se apunta
    // el puntero a null.
    if (read == -1) {
        free(*output);
        *output = NULL;
        *size = 0;
        // Si getline fallo porque se alcanzo EOF se retorna 0.
        if (fileReaderEOF(self))
            return 0;
        // Sino se retorna -1 indicando que fallo.
        return -1;
    }
    // Se pone el ultimo char en 0 para eliminar el '\n'.
    if (read > 0)
      (*output)[--read] = 0;

    return read;
}
