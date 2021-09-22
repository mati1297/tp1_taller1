#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "common_file_reader.h"

uint16_t fileReaderInit(FileReader * self, FILE * fds){
    if (!fds)
        return 1;
    self->fds = fds;
    return 0;
}

void fileReaderUnInit(FileReader * self){
    if (self->fds && self->fds != stdin)
        fclose(self->fds);
}

uint16_t fileReaderInitFromName(FileReader * self, char * name){
    if (!(self->fds = fopen(name, "r")))
        return 1;
    return 0;
}

bool fileReaderEOF(FileReader * self){
    return feof(self->fds);
}

ssize_t fileReaderReadLine(FileReader * self, char * output, size_t size){
    size_t _size = size;
    ssize_t read;
    char * buffer = (char *) malloc(_size);
    read = getline(&buffer, &_size, self->fds);
    buffer[--read] = 0;
    if(size < _size || _size == -1){
        free(buffer);
        buffer = NULL;
        return -1;
    }
    strncpy(output, buffer, size);
    free(buffer);
    buffer = NULL;
    return read;
}
