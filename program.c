#include "program.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Ends the program and returns exit status 1
void endProgram(const char *message) {
    printf("%s\n", message);
    exit(1);
}

// Checks the program running variables
void assertProgram() {
    bool *set = calloc(MAX_ASCII_DECIMAL_VALUE + 1, sizeof(bool));
    const char *VARIABLE_CHARS = ACCEPTED_VARIABLE_CHARS;
    const size_t ACCEPTED_VARIABLE_CHARS_COUNT = strlen(ACCEPTED_VARIABLE_CHARS);
    if (ACCEPTED_VARIABLE_CHARS_COUNT > MAX_VARIABLE_COUNT) {
        endProgram(DEFAULT_MAX_VARIABLE_EXCEEDED_ERROR_MESSAGE );
    }
    for (size_t i = 0; i < ACCEPTED_VARIABLE_CHARS_COUNT; i++) {
        char c = VARIABLE_CHARS[i];
        if (c == NEGATION_CHAR) {
            endProgram(DEFAULT_NEGATION_CHAR_AT_VARIABLE_ERROR_MESSAGE);
        }
        if (set[(size_t) c]) {
            endProgram(DEFAULT_REPEATED_ACCEPTED_VARIABLE_CHAR_ERROR_MESSAGE);
        }
        set[(size_t) c] = true;
    }
    free(set);
}
