//
// Created by matias on 15/9/21.
//

#ifndef FILE_READER_H
#define FILE_READER_H

#include <stdio.h>
#include <string.h>

typedef struct {
    FILE * fds;
} FileReader;

int fileReaderInit(FileReader * self, FILE * fds);
void fileReaderUnInit(FileReader * self);
int fileReaderInitFromName(FileReader * self, char * name);
bool fileReaderEOF(FileReader * self);
int fileReaderReadLine(FileReader * self, char * output, size_t size);


#endif