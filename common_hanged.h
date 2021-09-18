#ifndef TP1_TALLER1_COMMON_HANGED_H
#define TP1_TALLER1_COMMON_HANGED_H

#define MAX_WORD_LENGTH 50
#define UNKNOWN_CHARACTER '_'
#define MAX_ATTEMPTS 127
#define INFORMATION_PACK_HEADER_SIZE 3
#define MAX_LETTERS_PER_SENT 8

#define MASK_ATTEMPTS 127
#define MASK_STATE_IN_PROGRESS 127
#define MASK_STATE_FINISHED 128
#define MASK_STATE 128


#include "server_file_reader.h"

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
HangedState hangedGetState(Hanged * self);
size_t hangedGetAttempts(Hanged * self);
int hangedPackInformation(Hanged * self, char * buffer, size_t size);
unsigned short hangedUnpackInformationHeader(char * package, HangedState * state, short unsigned * attempts);
void hangedUnpackInformationWord(char * package, char * buffer, size_t size);

#endif //TP1_TALLER1_COMMON_HANGED_H
