#include "kmap.h"
#include "program.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

// Creates a Kmap struct based on given sum of products expression
Kmap *createKmap(char *expression, size_t length) {
    Kmap *kmap = malloc(sizeof(Kmap));
    if (kmap == NULL) {
        endProgram(DEFAULT_ALLOCATION_ERROR_MESSAGE);
    }
    kmap->variables = malloc((MAX_VARIABLE_COUNT + 1) * sizeof(char));
    if (kmap->variables == NULL) {
        endProgram(DEFAULT_ALLOCATION_ERROR_MESSAGE);
    }
    kmap->variable_map = malloc((MAX_ASCII_DECIMAL_VALUE + 1) * sizeof(size_t));
    if (kmap->variable_map == NULL) {
        endProgram(DEFAULT_ALLOCATION_ERROR_MESSAGE);
    }
    kmap->variable_count = 0;
    for (size_t i = 0; i < length && expression[i] != SUM_CHAR; i++) {
        char variable = expression[i];
        if (variable != NEGATION_CHAR) {
            kmap->variables[kmap->variable_count] = variable;
            kmap->variable_map[(size_t) variable] = kmap->variable_count;
            kmap->variable_count++;
        }
    }
    kmap->variables = realloc(kmap->variables, (kmap->variable_count + 1) * sizeof(char));
    if (kmap->variables == NULL) {
        endProgram(DEFAULT_ALLOCATION_ERROR_MESSAGE);
    }
    kmap->lines = 2;
    kmap->columns = 1 << (kmap->variable_count - 1);
    kmap->matrix = malloc(kmap->lines * sizeof(bool*));
    if (kmap->matrix == NULL) {
        endProgram(DEFAULT_ALLOCATION_ERROR_MESSAGE);
    }
    for (size_t i = 0; i < kmap->lines; i++) {
        kmap->matrix[i] = calloc(kmap->columns, sizeof(bool));
        if (kmap->matrix[i] == NULL) {
            endProgram(DEFAULT_ALLOCATION_ERROR_MESSAGE);
        }
    }
    bool *expression_value = malloc(kmap->variable_count * sizeof(bool));
    for (size_t i = 0; i < length; i++) {
        char variable = expression[i];
        if (variable == SUM_CHAR) {
            fillKmapCell(kmap, expression_value);
            continue;
        }
        if (variable != NEGATION_CHAR) {
            expression_value[kmap->variable_map[variable]] = 1;
        }
        else {
            i++;
            variable = expression[i];
            expression_value[kmap->variable_map[variable]] = 0;
        }
    }
    fillKmapCell(kmap, expression_value);
    free(expression_value);
}

// Puts 1 at a specific cell in the Kmap matrix based on given expression
void fillKmapCell(Kmap *kmap, bool *expression_value) {
    // 1011
}

// Frees a Kmap struct
void freeKmap(Kmap *kmap) {
    for (size_t i = 0; i < kmap->lines; i++) {
        free(kmap->matrix[i]);
    }
    free(kmap->matrix);
    free(kmap->variable_map);
    free(kmap);
}

// Returns the Gray Code of an integer
unsigned int toGrayCode(unsigned int n) {
    return n ^ (n >> 1);
}

// Returns an integer from a Gray Code
unsigned int fromGrayCode(unsigned int g) {
    unsigned int n = g;
    while (g >>= 1) n ^= g;
    return n;
}