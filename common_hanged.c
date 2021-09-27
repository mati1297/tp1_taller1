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

void hangedInit(Hanged * self, uint8_t attempts){
    self->attempts = (attempts < MAX_ATTEMPTS) ? attempts : MAX_ATTEMPTS;
    self->state = STATE_INACTIVE;
    self->victories = 0;
    self->defeats = 0;
    self->word = NULL;
    self->known_word = NULL;
}

void hangedUnInit(Hanged * self){
    if (self->word) {
        free(self->word);
        self->word = NULL;
    }
    if (self->known_word){
        free(self->known_word);
        self->known_word = NULL;
    }
}

uint8_t hangedAddWord(Hanged * self, char * word) {
    size_t word_size = strlen(word);
    if (word_size > MAX_WORD_LENGTH)
        return 1;
    for (int i = 0; i < strlen(word); i++) {
        if (word[i] < 'a' || word[i] > 'z')
            return 1;
    }
    if (self->word){
        free(self->word);
        self->word = NULL;
    }
    if (self->known_word){
        free(self->known_word);
        self->known_word = NULL;
    }
    if (!(self->word = malloc(word_size + 1)))
        return 1;
    if (!(self->known_word = malloc(word_size + 1)))
        return 1;
    memset(self->word, 0, word_size + 1);
    memset(self->word, 0, word_size + 1);
    strncpy(self->word, word, word_size + 1);
    memset(self->known_word, UNKNOWN_CHARACTER, word_size);

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
    } else if (!self->attempts_count) {
        self->state = STATE_PLAYER_LOSES;
        strncpy(self->known_word, self->word, strlen(self->known_word));
        self->defeats++;
    }
    return 0;
}

HangedState hangedGetState(Hanged * self){
    return self->state;
}

size_t hangedGetVictories(Hanged * self){
    return self->victories;
}

size_t hangedGetDefeats(Hanged * self){
    return self->defeats;
}

uint8_t hangedGetAttemptsCount(Hanged * self){
    return self->attempts_count;
}

const char * hangedGetKnownWord(Hanged * self){
    return self->known_word;
}


