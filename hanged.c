#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <netinet/in.h>
#include "hanged.h"

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

int hangedInit(Hanged * self, char * word, size_t attempts){
    if (strlen(word) > MAX_WORD_LENGTH)
        return 1;
    for (int i = 0; i < strlen(word); i++){
        if(word[i] < 'a' || word[i] > 'z')
            return 1;
    }
    memset(self->word, 0, MAX_WORD_LENGTH);
    memset(self->known_word, 0, MAX_WORD_LENGTH);
    strncpy(self->word, word, MAX_WORD_LENGTH);
    memset(self->known_word, UNKNOWN_CHARACTER, strlen(self->word));
    self->attempts = (attempts < MAX_ATTEMPTS) ? attempts : MAX_ATTEMPTS;
    self->state = STATE_IN_PROGRESS;
    return 0;
}

int hangedTryLetter(Hanged * self, char letter){
    if (letter < 'a' || letter > 'z')
        return 1;
    if (self->state != STATE_IN_PROGRESS)
        return 1;
    if (!_hangedCheckLetterAndReplaceIfNecessary(self, letter))
        self->attempts--;
    if (_hangedCheckPlayerWin(self))
        self->state = STATE_PLAYER_WINS;
    else if (!self->attempts)
        self->state = STATE_PLAYER_LOSES;
    return 0;
}

int hangedGetCorrectWord(Hanged * self, char * buffer, size_t size){
    if(size < (strlen(self->word) + 1))
        return 1;
    strncpy(buffer, self->word, size);
    return 0;
}

int hangedGetKnownWord(Hanged * self, char * buffer, size_t size){
    if(size < (strlen(self->known_word) + 1))
        return 1;
    strncpy(buffer, self->known_word, size);
    return 0;
}

HangedState hangedGetState(Hanged * self){
    return self->state;
}

size_t hangedGetAttempts(Hanged * self){
    return self->attempts;
}

//UN POCO HARCODEADA, DESHARCODEARLA. ESTA Y LA SIGUIENTE
int hangedPackInformation(Hanged * self, char * package, size_t size){
    int requiredSize = strlen(self->known_word) + INFORMATION_HEADER_SIZE;
    if(size < requiredSize + 1) // VER LO DEL + 1 DESPUES
        return 1;
    package[0] = (char) (self->attempts) & MASK_ATTEMPTS;
    if(self->state == STATE_IN_PROGRESS)
        package[0] &= MASK_STATE_IN_PROGRESS;
    else
        package[0] |= MASK_STATE_FINISHED;
    unsigned short string_length = (unsigned short) strlen(self->word);
    string_length = htons(string_length);
    memcpy(&package[1], (const char *) &string_length, sizeof(unsigned short));
    strncpy(&package[3], self->known_word, strlen(self->known_word));
    return 0;
}

int hangedUnpackInformation(char * package, HangedState * state, short unsigned * attempts, char * buffer, size_t size){
    *attempts = (unsigned short) package[0] & MASK_ATTEMPTS;
    printf("%d\n", *attempts);
    char _state = (char) (package[0] & MASK_STATE) >> 7;
    if(_state){
        if(*attempts > 0)
            *state = STATE_PLAYER_WINS;
        else
            *state = STATE_PLAYER_LOSES;
    } else {
        *state = STATE_IN_PROGRESS;
    }
    printf("%d\n", *state);

    unsigned short string_length = 0;
    memcpy((char *) &string_length, &package[1], sizeof(unsigned short));
    string_length = ntohs(string_length);
    if (size < string_length)
        return 1;
    memset(buffer, 0, size);
    strncpy(buffer, &package[3], string_length);
    printf("%s\n", buffer);
    return 0;
}
