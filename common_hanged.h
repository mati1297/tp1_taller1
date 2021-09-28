#ifndef TP1_TALLER1_COMMON_HANGED_H
#define TP1_TALLER1_COMMON_HANGED_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "common_file_reader.h"

#define UNKNOWN_CHARACTER '_'
#define MAX_ATTEMPTS 127
#define MAX_WORD_LENGTH 65535

// Tipo HangedState, permite conocer el estado del TDA Hanged.
typedef enum{
    STATE_IN_PROGRESS,
    STATE_PLAYER_WINS,
    STATE_PLAYER_LOSES,
} HangedState;

// Tipo Hanged, permite jugar una o varias partidas del juego
// ahorcado.
typedef struct{
    char * word; // palabra real.
    char * known_word; // palabra conocida hasta ahora.
    uint8_t attempts_count, attempts;
    HangedState state;
    size_t victories, defeats;
} Hanged;

/* Inicializa la instancia self. Se le deben pasar la
 * cantidad de intentos que se permiten en la partida.
 * Pre: self debe apuntar a una direccion de memoria valida.
 *      attempts debe ser menor a 127.  */
void hangedInit(Hanged * self, uint8_t attempts);

/* Desinicializa la instancia self.
 * Pre: self debe apuntar a una direccion de memoria valida.  */
void hangedUnInit(Hanged * self);

/* Agrega una palabra desde word al juego, pisando la palabra
 * que se encontrara antes y reiniciandolo. Retorna 0 en caso
 * de exito o 1 en caso contrario.
 * Pre: self debe apuntar a una direccion de memoria valida.
 *      word debe apuntar a un array de chars con caracteres
 *      validos (letras minusculas).  */
uint8_t hangedAddWord(Hanged * self, char * word);

/* Prueba la letra letter en el juego, y si es necesario cambia
 * el estado del juego a victoria o derrota. Retorna 0 en caso
 * de exito o 0 en caso contrario.
 * Pre: self debe apuntar a una direccion de memoria valida.
 *      letter debe ser una letra valida (letra minuscula),
 *      sino no se hara nada.  */
uint8_t hangedTryLetter(Hanged * self, char letter);

/* Retorna el estado del juego.
 * Pre: self debe apuntar a una direccion de memoria valida. */
HangedState hangedGetState(Hanged * self);

/* Retorna las victorias registradas en el juego.
 * Pre: self debe apuntar a una direccion de memoria valida.  */
size_t hangedGetVictories(Hanged * self);

/* Retorna las derrotas registradas en el juego.
 * Pre: self debe apuntar a una direccion de memoria valida.  */
size_t hangedGetDefeats(Hanged * self);

/* Retorna la cantidad de intentos restantes.
 * Pre: self debe apuntar a una direccion de memoria valida.  */
uint8_t hangedGetAttemptsCount(Hanged * self);

/* Retorna un puntero a char constante que apunta a la palabra conocida
 * hasta el momento.
 * Pre: self debe apuntar a una direccion de memoria valida. */
const char * hangedGetKnownWord(Hanged * self);



#endif //TP1_TALLER1_COMMON_HANGED_H
