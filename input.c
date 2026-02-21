#include "input.h"
#include "program.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

static const char *VARIABLE_CHARS = ACCEPTED_VARIABLE_CHARS;
static const size_t VARIABLE_CHARS_COUNT = strlen(ACCEPTED_VARIABLE_CHARS);

// Creates a Buffer struct with a specified size for its char *content
Buffer *createBuffer(size_t size) {
    Buffer *buffer = malloc(sizeof(Buffer));
    if (buffer == NULL) {
        endProgram(DEFAULT_ALLOCATION_ERROR_MESSAGE);
    }
    buffer->size = size;
    buffer->content = calloc(buffer->size, sizeof(char));
    if (buffer->content == NULL)  {
        endProgram(DEFAULT_ALLOCATION_ERROR_MESSAGE);
    }
    return buffer;
}

// Creates a VariableSet struct with a specified size for its bool **content
VariableSet *createVariableSet(size_t size) {
    VariableSet *variable_set = malloc(sizeof(VariableSet));
    if (variable_set == NULL) {
        endProgram(DEFAULT_ALLOCATION_ERROR_MESSAGE);
    }
    variable_set->size = size;
    variable_set->content = malloc(variable_set->size * sizeof(bool*));
    if (variable_set->content == NULL) {
        endProgram(DEFAULT_ALLOCATION_ERROR_MESSAGE);
    }
    for (size_t i = 0; i < variable_set->size; i++) {
        variable_set->content[i] = calloc(MAX_ASCII_DECIMAL_VALUE + 1, sizeof(bool));
        if (variable_set->content[i] == NULL) {
            endProgram(DEFAULT_ALLOCATION_ERROR_MESSAGE);
        }
    }
    return variable_set;
}

// Frees a Buffer struct
void freeBuffer(Buffer *buffer) {
    free(buffer->content);
    free(buffer);
}

// Frees a VariableSet struct
void freeVariableSet(VariableSet *variable_set) {
    for (size_t i = 0; i < variable_set->size; i++) {
        free(variable_set->content[i]);
    }
    free(variable_set->content);
    free(variable_set);
}

// Reads a line from a text file and saves its content in the Buffer content
void readInput(Buffer *buffer, const char *file_name) {
    FILE *input_file = fopen(file_name, "r");
    if (input_file == NULL) {
        endProgram(DEFAULT_ALLOCATION_ERROR_MESSAGE);
    }
    fgets(buffer->content, buffer->size * sizeof(char), input_file);
    fclose(input_file);
    buffer->length = strlen(buffer->content);
    if (buffer->length == 0) {
        endProgram(DEFAULT_EMPTY_INPUT_ERROR_MESSAGE);
    }
}

// Trims a Buffer content, removing all the spaces (ASCII 32)
void removeSpaces(Buffer *buffer) {
    size_t non_space_count = 0;
    for (size_t i = 0; i < buffer->length; i++) {
        if (buffer->content[i] != ' ') {
            non_space_count++;
        }
    }
    size_t l = 0;
    size_t r = 1;
    while (l < non_space_count) {
        if (buffer->content[l] != ' ') {
            l++;
            r++;
            continue;
        }
        while (r < buffer->length && buffer->content[r] == ' ') r++;
        buffer->content[l] = buffer->content[r];
        buffer->content[r] = ' ';
        l++;
        r++;
    }
    buffer->length = non_space_count;
    buffer->size = non_space_count + 1;
    buffer->content = realloc(buffer->content, buffer->size * sizeof(char));
    if (buffer->content == NULL) {
        endProgram(DEFAULT_ALLOCATION_ERROR_MESSAGE);
    }
    buffer->content[buffer->length] = '\0';
}

// Validate a sum of products expression
void validateExpression(Buffer *buffer) {
    removeSpaces(buffer);
    checkValidSumExpression(buffer);
    size_t product_count = getProductCount(buffer);
    VariableSet *variable_set = createVariableSet(product_count);
    checkValidProductExpressions(buffer, variable_set);
    checkEqualVariablesBetweenProducts(variable_set);
    freeVariableSet(variable_set);
}

// Checks if the Buffer content contains a valid sum between seperate expressions
void checkValidSumExpression(Buffer *buffer) {
    if (buffer->content[0] == SUM_CHAR || buffer->content[buffer->length - 1] == SUM_CHAR) {
        endProgram(DEFAULT_SUM_SYMBOL_START_END_ERROR_MESSAGE);
    }
    for (size_t i = 0; i < buffer->length - 1; i++) {
        if (buffer->content[i] == SUM_CHAR && buffer->content[i + 1] == SUM_CHAR) {
            endProgram(DEFAULT_SUM_SYMBOL_EMPTY_ERROR_MESSAGE);
        }
    }
}

// Returns the amount of products contained in the Buffer content
size_t getProductCount(Buffer *buffer) {
    size_t product_count = 0;
    for (size_t i = 0; i < buffer->length; i++) {
        if (buffer->content[i] == SUM_CHAR) {
            product_count++;
        }
    }
    return product_count + 1;
}

// Checks if every product expression in the Buffer content is valid
void checkValidProductExpressions(Buffer *buffer, VariableSet *variable_set) {
    size_t product_id = 0;
    size_t l = 0;
    for (size_t r = 1; r < buffer->length; r++) {
        if (buffer->content[r] == SUM_CHAR) {
            if (!isValidProductExpression(variable_set, product_id, buffer->content, l, r - 1)) {
                endProgram(DEFAULT_INVALID_PRODUCT_ERROR_MESSAGE);
            }
            product_id++;
            l = r + 1;
            r++;
        }
    }
    if (!isValidProductExpression(variable_set, product_id, buffer->content, l, buffer->length - 1)) {
        endProgram(DEFAULT_INVALID_PRODUCT_ERROR_MESSAGE);
    }
}

// Checks if a product expression is valid
bool isValidProductExpression(VariableSet *set, size_t line, char *string, size_t l, size_t r) {
    bool reading_not = false;
    for (size_t i = l; i <= r; i++) {
        char c = string[i];
        if (c != NEGATION_CHAR && !isValidVariableChar(c)) {
            endProgram(DEFAULT_INVALID_PRODUCT_ERROR_MESSAGE);
        }
        if (c == NEGATION_CHAR) {
            reading_not = true;
            continue;
        }
        if (set->content[line][(size_t) c]) {
            endProgram(DEFAULT_REPETEAD_VARIABLE_ERROR_MESSAGE);
        }
        set->content[line][(size_t) c] = true;
        reading_not = false;
    }
    return !reading_not;
}

// Checks if accepted variable chars string contains c
bool isValidVariableChar(char c) {
    for (size_t i = 0; i < VARIABLE_CHARS_COUNT; i++) {
        if (c == VARIABLE_CHARS[i]) {
            return true;
        }
    }
    return false;
}

// Checks if all products share the same set of variables
void checkEqualVariablesBetweenProducts(VariableSet *variable_set) {
    for (size_t i = 0; i <= MAX_ASCII_DECIMAL_VALUE; i++) {
        bool pivot = variable_set->content[0][i];
        for (size_t j = 1; j < variable_set->size; j++) {
            if (variable_set->content[j][i] != pivot) {
                endProgram(DEFAULT_VARIABLE_SET_DIFFERS_ERROR_MESSAGE);
            }
        }
    }
}
