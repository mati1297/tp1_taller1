#ifndef FILE_READER_H
#define FILE_READER_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

// Tipo FileReader, permite leer desde un archivo.
typedef struct {
    FILE * file;
} FileReader;

/* Inicializa la instancia self para ser utilizada a partir de una
 * estructura FILE. Retorna 0 en caso exitoso o 1 en caso de error.
 * Pre: self debe apuntar a una direccion de memoria valida.
 *      file es un puntero a una estrura FILE valida.  */
uint8_t fileReaderInit(FileReader * self, FILE * file);

/* Inicializa la instancia self para ser utilizada a partir de un
 * array de chars con el nombre del archivo. Retorna 0 en caso
 * exitoso o 1 en caso de error.
 * Pre: self debe apuntar a una direccion de memoria valida.
 *      name debe apuntar a una direccion de memoria valida.  */
uint8_t fileReaderInitFromName(FileReader * self, char * name);

/*  Desinicializa la instancia self. Retorna 1 en caso de error.
 *  Pre: self debe apuntar a una direccion de memoria valida.  */
void fileReaderUnInit(FileReader * self);

/*  Retorna true si se alcanzo el END OF FILE del archivo,
 *  retorna false en caso contrario.
 *  Pre: self debe apuntar a una direccion de memoria valida.  */
bool fileReaderEOF(FileReader * self);

/*  Lee una linea del archivo abierto. Pide memoria y la asigna al puntero output,
 *  y guarda la cantidad pedida en la direccion apuntada por size. Retorna
 *  la cantidad leida o -1 en caso de error.
 *  Pre: self debe apuntar a una direccion de memoria valida.
 *       output debe apuntar a una direccion de memoria valida.
 *       size debe apuntar a una direccion de memoria valida.  */
ssize_t fileReaderReadLine(FileReader * self, char ** output, size_t * size);


#endif
