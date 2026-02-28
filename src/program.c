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

// Returns a pointer to a block of memory reserved by malloc
void *newMalloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) endProgram(DEFAULT_MEMORY_ALLOCATION_ERROR_MESSAGE);
    return ptr;
}

// Returns a pointer to a block of memory reserved by calloc
void *newCalloc(size_t amount, size_t size) {
    void *ptr = calloc(amount, size);
    if (ptr == NULL) endProgram(DEFAULT_MEMORY_ALLOCATION_ERROR_MESSAGE);
    return ptr;
}

// Returns a pointer to a block of memory reserved by realloc
void *newRealloc(void *ptr, size_t size) {
    void *nptr = realloc(ptr, size);
    if (nptr == NULL) endProgram(DEFAULT_MEMORY_ALLOCATION_ERROR_MESSAGE);
    return nptr;
}

// Checks the program's running variables
void assertProgram() {
    bool *set = newCalloc(MAX_ASCII_DECIMAL_VALUE + 1, sizeof(bool));
    const char *VARIABLE_CHARS = ACCEPTED_VARIABLE_CHARS;
    const size_t ACCEPTED_VARIABLE_CHARS_COUNT = strlen(ACCEPTED_VARIABLE_CHARS);
    if (ACCEPTED_VARIABLE_CHARS_COUNT > MAX_VARIABLE_COUNT) {
        endProgram(DEFAULT_MAX_VARIABLE_AMOUNT_EXCEEDED_ERROR_MESSAGE);
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
