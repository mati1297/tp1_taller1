//
// Created by matias on 15/9/21.
//

#ifndef FILE_READER_H
#define FILE_READER_H

#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 20


typedef struct {
    FILE * fds;
    char buffer[BUFFER_SIZE];
} FileReader;

int fileReaderInit(FileReader * self, FILE * fds);
void fileReaderUnInit(FileReader * self);
int fileReaderInitFromName(FileReader * self, char * name);
bool fileReaderEOF(FileReader * self);
int fileReaderReadLine(FileReader * self, char * output, size_t size);


#endif
