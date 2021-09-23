#ifndef TP1_TALLER1_COMMON_HANGED_H
#define TP1_TALLER1_COMMON_HANGED_H

#define MAX_WORD_LENGTH 50
#define UNKNOWN_CHARACTER '_'
#define MAX_ATTEMPTS 127
#define INFORMATION_PACK_HEADER_SIZE 3


#define MASK_ATTEMPTS 127
#define MASK_STATE_IN_PROGRESS 127
#define MASK_STATE_FINISHED 128
#define MASK_STATE 128

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "common_file_reader.h"

typedef enum{
    STATE_IN_PROGRESS,
    STATE_PLAYER_WINS,
    STATE_PLAYER_LOSES,
    STATE_INACTIVE
} HangedState;

typedef struct{
    char word[MAX_WORD_LENGTH + 1];
    char known_word[MAX_WORD_LENGTH + 1];
    uint16_t attempts_count, attempts;
    HangedState state;
    size_t victories, defeats;
} Hanged;


void hangedInit(Hanged * self, uint16_t attempts);
uint8_t hangedAddWord(Hanged * self, char * word);
uint8_t hangedTryLetter(Hanged * self, char letter);
uint8_t hangedGetCorrectWord(Hanged * self, char * buffer, size_t size);
uint8_t hangedGetKnownWord(Hanged * self, char * buffer, size_t size);
HangedState hangedGetState(Hanged * self);
uint16_t hangedGetAttempts(Hanged * self);
size_t hangedGetVictories(Hanged * self);
size_t hangedGetDefeats(Hanged * self);
ssize_t hangedPackInformation(Hanged * self, char * buffer, size_t size);
uint16_t hangedUnpackInformationHeader(char * package, HangedState * state, uint16_t * attempts);
void hangedUnpackInformationWord(char * package, char * buffer, size_t size);

#endif //TP1_TALLER1_COMMON_HANGED_H
