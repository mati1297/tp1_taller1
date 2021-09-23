#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <netinet/in.h> //borrar luego
#include "common_hanged.h"

static bool _hangedCheckLetterAndReplaceIfNecessary(Hanged * self, char letter){
    bool is_there = false;
    for (int i = 0; i < strlen(self->word); i++){
        if (self->word[i] == letter){
            is_there = true;
            break;
        }
    }
    if (!is_there)
        return false;
    for (int i = 0; i < strlen(self->word); i++){
        if (self->word[i] == letter)
            self->known_word[i] = letter;
    }
    return true;
}

static bool _hangedCheckPlayerWin(Hanged * self){
    for (int i = 0; i < strlen(self->known_word); i++){
        if (self->known_word[i] == UNKNOWN_CHARACTER)
            return false;
    }
    return true;
}

void hangedInit(Hanged * self, uint16_t attempts){
    self->attempts = (attempts < MAX_ATTEMPTS) ? attempts : MAX_ATTEMPTS;
    self->state = STATE_INACTIVE;
    self->victories = 0;
    self->defeats = 0;
}

uint8_t hangedAddWord(Hanged * self, char * word) {
    if (strlen(word) > MAX_WORD_LENGTH)
        return 1;
    for (int i = 0; i < strlen(word); i++) {
        if (word[i] < 'a' || word[i] > 'z')
            return 1;
    }
    memset(self->word, 0, MAX_WORD_LENGTH);
    memset(self->known_word, 0, MAX_WORD_LENGTH);
    strncpy(self->word, word, MAX_WORD_LENGTH);
    memset(self->known_word, UNKNOWN_CHARACTER, strlen(self->word));

    self->attempts_count = self->attempts;
    self->state = STATE_IN_PROGRESS;
    return 0;
}

uint8_t hangedTryLetter(Hanged * self, char letter){
    if (letter < 'a' || letter > 'z')
        return 1;
    if (self->state != STATE_IN_PROGRESS)
        return 1;
    if (!_hangedCheckLetterAndReplaceIfNecessary(self, letter))
        self->attempts_count--;
    if (_hangedCheckPlayerWin(self)) {
        self->state = STATE_PLAYER_WINS;
        self->victories++;
    }
    else if (!self->attempts_count) {
        self->state = STATE_PLAYER_LOSES;
        strncpy(self->known_word, self->word, strlen(self->known_word));
        self->defeats++;
    }
    return 0;
}

uint8_t hangedGetCorrectWord(Hanged * self, char * buffer, size_t size){
    if(size < (strlen(self->word) + 1))
        return 1;
    strncpy(buffer, self->word, size);
    return 0;
}

uint8_t hangedGetKnownWord(Hanged * self, char * buffer, size_t size){
    if(size < (strlen(self->known_word) + 1))
        return 1;
    strncpy(buffer, self->known_word, size);
    return 0;
}

HangedState hangedGetState(Hanged * self){
    return self->state;
}

uint16_t hangedGetAttempts(Hanged * self){
    return self->attempts_count;
}

size_t hangedGetVictories(Hanged * self){
    return self->victories;
}

size_t hangedGetDefeats(Hanged * self){
    return self->defeats;
}

//UN POCO HARCODEADA, DESHARCODEARLA. ESTA Y LA SIGUIENTE
ssize_t hangedPackInformation(Hanged * self, char * package, size_t size){
    int requiredSize = strlen(self->known_word) + INFORMATION_PACK_HEADER_SIZE;
    if(size < requiredSize + 1) // VER LO DEL + 1 DESPUES
        return -1;
    package[0] = (char) (self->attempts_count) & MASK_ATTEMPTS;
    if(self->state == STATE_IN_PROGRESS)
        package[0] &= MASK_STATE_IN_PROGRESS;
    else
        package[0] |= MASK_STATE_FINISHED;
    unsigned short string_length = (unsigned short) strlen(self->word);
    string_length = htons(string_length);
    memcpy(&package[1], (const char *) &string_length, sizeof(unsigned short));
    strncpy(&package[3], self->known_word, ntohs(string_length));
    return INFORMATION_PACK_HEADER_SIZE + strlen(self->known_word);
}

uint16_t hangedUnpackInformationHeader(char * package, HangedState * state, uint16_t * attempts){
    *attempts = (unsigned short) package[0] & MASK_ATTEMPTS;
    char _state = (char) (package[0] & MASK_STATE) >> 7;
    if(_state){
        if(*attempts > 0)
            *state = STATE_PLAYER_WINS;
        else
            *state = STATE_PLAYER_LOSES;
    } else {
        *state = STATE_IN_PROGRESS;
    }
    unsigned short string_length = 0;
    memcpy((char *) &string_length, &package[1], sizeof(unsigned short));
    string_length = ntohs(string_length);
    return string_length;
}

void hangedUnpackInformationWord(char * package, char * buffer, size_t size){
    memset(buffer, 0, size);
    strncpy(buffer, package, size);
    buffer[size] = 0;
}
