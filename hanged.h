#ifndef TP1_TALLER1_HANGED_H
#define TP1_TALLER1_HANGED_H

#define MAX_WORD_LENGTH 50
#define UNKNOWN_CHARACTER '_'
#define MAX_ATTEMPTS 30
#define INFORMATION_HEADER_SIZE 3

#define MASK_ATTEMPTS 127
#define MASK_STATE_IN_PROGRESS 127
#define MASK_STATE_FINISHED 128
#define MASK_STATE 128


#include "file_reader.h"

typedef enum{
    STATE_IN_PROGRESS,
    STATE_PLAYER_WINS,
    STATE_PLAYER_LOSES,
} HangedState;

typedef struct{
    char word[MAX_WORD_LENGTH + 1];
    char known_word[MAX_WORD_LENGTH + 1];
    unsigned short attempts;
    HangedState state;
} Hanged;

int hangedInit(Hanged * self, char * word, size_t attempts);
int hangedTryLetter(Hanged * self, char letter);
int hangedGetCorrectWord(Hanged * self, char * buffer, size_t size);
int hangedGetKnownWord(Hanged * self, char * buffer, size_t size);
int hangedPackInformation(Hanged * self, char * buffer, size_t size);
int hangedUnpackInformation(char * package, HangedState * state, short unsigned * attempts, char * buffer, size_t size);

#endif //TP1_TALLER1_HANGED_H
