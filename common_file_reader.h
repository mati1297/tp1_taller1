#ifndef FILE_READER_H
#define FILE_READER_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    FILE * fds;
} FileReader;

uint16_t fileReaderInit(FileReader * self, FILE * fds);
void fileReaderUnInit(FileReader * self);
uint16_t fileReaderInitFromName(FileReader * self, char * name);
bool fileReaderEOF(FileReader * self);
ssize_t fileReaderReadLine(FileReader * self, char ** output, size_t * size);


#endif
