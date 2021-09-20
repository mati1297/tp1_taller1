#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "server_file_reader.h"

int fileReaderInit(FileReader * self, FILE * fds){
    if (!fds || fds == stdin)
        return 1;
    self->fds = fds;
    return 0;
}

void fileReaderUnInit(FileReader * self){
    if (self->fds)
        fclose(self->fds);
}

int fileReaderInitFromName(FileReader * self, char * name){
    if (!(self->fds = fopen(name, "r")))
        return 1;
    return 0;
}

bool fileReaderEOF(FileReader * self){
    return feof(self->fds);
}

int fileReaderReadLine(FileReader * self, char * output, size_t size){
    size_t _size = size;
    ssize_t read;
    char * buffer = (char *) malloc(_size);
    read = getline(&buffer, &_size, self->fds);
    buffer[--read] = 0;
    printf("%s\n", buffer);
    if(size < _size || _size < 1){
        free(buffer);
        buffer = NULL;
        return -1;
    }
    strncpy(output, buffer, size);
    free(buffer);
    buffer = NULL;
    return read;
}
