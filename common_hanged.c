#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <netinet/in.h> //borrar luego
#include "common_hanged.h"

/* Chequea si la letra se encuentra en la palabra y si lo esta, reemplaza
 * esa letra en la palabra conocida. */
static bool _hangedCheckLetterAndReplaceIfNecessary(Hanged * self, char letter){
    bool is_there = false;
    // Se chequea si la letra se encuentra en la palabra.
    for (int i = 0; i < strlen(self->word); i++){
        if (self->word[i] == letter){
            is_there = true;
            break;
        }
    }
    // Si no esta se retorna falso.
    if (!is_there)
        return false;
    // Si esta se reemplaza todas las veces que haga falta.
    for (int i = 0; i < strlen(self->word); i++){
        if (self->word[i] == letter)
            self->known_word[i] = letter;
    }
    return true;
}

/* Chequea el estado de la palabra conocida y a partir de alli
 * cambia el estado segun corresponda y actualiza los contadores
 * de victorias y derrotas */
static void _hangedUpdateState(Hanged * self){
    // Si la palabra conocida y la real son iguales, el jugador
    // gana. En caso contrario y si no quedan mas intentos,
    // jugador pierde. Si el jugador pierde, se revela la palabra.
    if (!strcmp(self->word, self->known_word)) {
        self->state = STATE_PLAYER_WINS;
        self->victories++;
    } else if (!self->attempts_count) {
        self->state = STATE_PLAYER_LOSES;
        strncpy(self->known_word, self->word, strlen(self->known_word));
        self->defeats++;
    }
}

void hangedInit(Hanged * self, uint8_t attempts){
    self->attempts = (attempts < MAX_ATTEMPTS) ? attempts : MAX_ATTEMPTS;
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
    // Se chequea que la palabra no tenca caracteres invalidos.
    for (int i = 0; i < strlen(word); i++) {
        if (word[i] < 'a' || word[i] > 'z')
            return 1;
    }

    // Se pide memoria para la nueva palabra. Se utiliza el heap
    // porque la palabra puede ser de hasta 65535 bytes, por
    // lo que no es conveniente guardarlo en el stack.
    if (!(self->word = realloc(self->word, word_size + 1))) {
        // Si realloc falla deja la memoria anterior como si nada, por eso
        // se libera la memoria.
        free(self->word);
        self->word = NULL;
        free(self->known_word);
        self->known_word = NULL;
        return 1;
    }
    if (!(self->known_word = realloc(self->known_word, word_size + 1))) {
        free(self->word);
        self->word = NULL;
        free(self->known_word);
        self->known_word = NULL;
        return 1;
    }

    // Se setea a 0 la memoria pedida
    memset(self->word, 0, word_size + 1);
    memset(self->known_word, 0, word_size + 1);
    // Se copia la palabra y se setea la palabra desconocida
    // con el caracter que indica desconocido.
    strncpy(self->word, word, word_size + 1);
    memset(self->known_word, UNKNOWN_CHARACTER, word_size);

    // Se resetean los intentos y se pasa a estado en progreso.
    self->attempts_count = self->attempts;
    self->state = STATE_IN_PROGRESS;
    return 0;
}

uint8_t hangedTryLetter(Hanged * self, char letter){
    // Si la letra es invalida se ignora.
    if (letter < 'a' || letter > 'z')
        return 0;
    // Si el juego no esta en progreso se deuvelve un error.
    if (self->state != STATE_IN_PROGRESS)
        return 1;
    // Se chequea si la letra se encuentra y se reemplaza
    // de ser necesario.
    if (!_hangedCheckLetterAndReplaceIfNecessary(self, letter))
        self->attempts_count--;
    _hangedUpdateState(self);
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


