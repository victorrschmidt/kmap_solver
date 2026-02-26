#include "input.h"
#include "program.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

static const char *VARIABLE_CHARS = ACCEPTED_VARIABLE_CHARS;
static const size_t VARIABLE_CHARS_COUNT = strlen(ACCEPTED_VARIABLE_CHARS);

// Creates a Buffer struct
Buffer *createBuffer() {
    Buffer *buffer = newMalloc(sizeof(Buffer));
    buffer->size = MAX_BUFFER_SIZE;
    buffer->content = newCalloc(buffer->size, sizeof(char));
    return buffer;
}

// Creates a VariableSet struct with a specified size for its bool **content
VariableSet *createVariableSet(size_t size) {
    VariableSet *variable_set = newMalloc(sizeof(VariableSet));
    variable_set->size = size;
    variable_set->accepted_variables = newCalloc(MAX_ASCII_DECIMAL_VALUE + 1, sizeof(bool));
    for (size_t i = 0; i < VARIABLE_CHARS_COUNT; i++) {
        size_t c = (size_t) VARIABLE_CHARS[i];
        variable_set->accepted_variables[c] = 1;
    }
    variable_set->content = newMalloc(variable_set->size * sizeof(bool*));
    for (size_t i = 0; i < variable_set->size; i++) {
        variable_set->content[i] = newCalloc(MAX_ASCII_DECIMAL_VALUE + 1, sizeof(bool));
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
    free(variable_set->accepted_variables);
    free(variable_set);
}

// Reads a line from a text file and saves its content in the Buffer content
void readInput(Buffer *buffer) {
    FILE *input_file = fopen(INPUT_FILE_NAME, "r");
    if (input_file == NULL) endProgram(DEFAULT_ALLOCATION_ERROR_MESSAGE);
    fgets(buffer->content, buffer->size * sizeof(char), input_file);
    fclose(input_file);
    buffer->length = strlen(buffer->content);
    if (buffer->length == 0) endProgram(DEFAULT_EMPTY_INPUT_ERROR_MESSAGE);
}

// Trims a Buffer content, removing all the spaces
void removeSpaces(Buffer *buffer) {
    size_t non_space_count = 0;
    for (size_t i = 0; i < buffer->length; i++) {
        if (!isspace(buffer->content[i])) {
            non_space_count++;
        }
    }
    size_t l = 0;
    size_t r = 1;
    while (l < non_space_count) {
        if (!isspace(buffer->content[l])) {
            l++;
            r++;
            continue;
        }
        while (r < buffer->length && isspace(buffer->content[r])) r++;
        buffer->content[l] = buffer->content[r];
        buffer->content[r] = ' ';
        l++;
        r++;
    }
    buffer->length = non_space_count;
    buffer->size = non_space_count + 1;
    buffer->content = newRealloc(buffer->content, buffer->size * sizeof(char));
    buffer->content[buffer->length] = '\0';
}

// Validate a sum of products expression
void validateExpression(Buffer *buffer) {
    removeSpaces(buffer);
    checkValidSumExpression(buffer);
    size_t product_count = getProductCount(buffer);
    VariableSet *variable_set = createVariableSet(product_count);
    checkAllValidProductExpressions(buffer, variable_set);
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
            endProgram(DEFAULT_DOUBLE_SUM_SYMBOL_ERROR_MESSAGE);
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
void checkAllValidProductExpressions(Buffer *buffer, VariableSet *variable_set) {
    size_t product_id = 0;
    size_t l = 0;
    for (size_t r = 1; r < buffer->length; r++) {
        if (buffer->content[r] == SUM_CHAR) {
            checkValidProductExpression(variable_set, product_id, buffer->content, l, r - 1);
            product_id++;
            l = r + 1;
            r++;
        }
    }
    checkValidProductExpression(variable_set, product_id, buffer->content, l, buffer->length - 1);
}

// Checks if a product expression is valid
void checkValidProductExpression(VariableSet *variable_set, size_t id, char *string, size_t l, size_t r) {
    bool is_reading_negation = false;
    size_t variable_count = 0;
    for (size_t i = l; i <= r; i++) {
        char c = string[i];
        if (c != NEGATION_CHAR && !isValidVariableChar(variable_set, c)) {
            endProgram(DEFAULT_INVALID_VARIABLE_ERROR_MESSAGE);
        }
        if (c == NEGATION_CHAR) {
            is_reading_negation = true;
            continue;
        }
        if (variable_set->content[id][(size_t) c]) {
            endProgram(DEFAULT_REPETEAD_VARIABLE_ERROR_MESSAGE);
        }
        variable_set->content[id][(size_t) c] = true;
        variable_count++;
        is_reading_negation = false;
    }
    if (is_reading_negation) {
        endProgram(DEFAULT_EMPTY_NEGATION_ERROR_MESSAGE);
    }
    if (variable_count < 2) {
        endProgram(DEFAULT_SINGLE_VARIABLE_ERROR_MESSAGE);
    }
}

// Checks if the accepted variable chars string contains c
bool isValidVariableChar(VariableSet *variable_set, char c) {
    return variable_set->accepted_variables[(size_t) c];
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
