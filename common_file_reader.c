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

ssize_t fileReaderReadLine(FileReader * self, char ** output, size_t * size){
    ssize_t read;
    read = getline(output, size, self->fds);
    if (read == -1) {
        free(*output);
        *output = NULL;
        *size = 0;
        if (fileReaderEOF(self))
            return 0;
        return -1;
    }
    if (read > 0)
      (*output)[--read] = 0;

    return read;
}
