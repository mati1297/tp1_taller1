#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "file_reader.h"


static int _fileReaderGetEndOfLinePosition(char * buffer){
    for (int i = 0; i < BUFFER_SIZE - 1; i++){
        if (buffer[i] == '\n')
            return i;
    }
    return -1;
}

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

int fileReaderReadLine(FileReader * self, char * output, int size){
    int readed = 0, eol_position;
    int fds_current_position = ftell(self->fds);
    bool state_finished = false;
    while (!feof(self->fds)
            && !ferror(self->fds) && readed < (size - 1) && !state_finished){
        int cant_to_read = ((BUFFER_SIZE < (size - readed)) ?
                (BUFFER_SIZE - 1) : (size - readed - 1));
        int n = fread(self->buffer, sizeof(char), cant_to_read, self->fds);
        self->buffer[n] = '\0';
        if ((eol_position = _fileReaderGetEndOfLinePosition(self->buffer)) >= 0) {
            fseek(self->fds,
                  fds_current_position + readed + eol_position + 1, SEEK_SET);
            self->buffer[eol_position] = '\0';
            state_finished = true;
        }
        strncpy(&output[readed], self->buffer, cant_to_read);
        readed += n;
    }
    output[readed] = '\0';
    if (ferror(self->fds))
        return -1;
    return readed;
}
