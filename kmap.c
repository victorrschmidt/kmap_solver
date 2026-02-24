#include "kmap.h"
#include "program.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Creates a Kmap struct based on given sum of products expression
Kmap *createKmap(char *expression, size_t length) {
    Kmap *kmap = newMalloc(sizeof(Kmap));
    kmap->variables = newMalloc((MAX_VARIABLE_COUNT + 1) * sizeof(char));
    kmap->variable_map = newMalloc((MAX_ASCII_DECIMAL_VALUE + 1) * sizeof(size_t));
    kmap->variable_count = 0;
    for (size_t i = 0; i < length && expression[i] != SUM_CHAR; i++) {
        char variable = expression[i];
        if (variable != NEGATION_CHAR) {
            kmap->variables[kmap->variable_count] = variable;
            kmap->variable_map[(size_t) variable] = kmap->variable_count;
            kmap->variable_count++;
        }
    }
    kmap->variables = newRealloc(kmap->variables, (kmap->variable_count + 1) * sizeof(char));
    kmap->variables[kmap->variable_count] = '\0';
    kmap->lines = 2;
    kmap->columns = 1 << (kmap->variable_count - 1);
    kmap->matrix = newMalloc(kmap->lines * sizeof(bool*));
    for (size_t i = 0; i < kmap->lines; i++) {
        kmap->matrix[i] = newCalloc(kmap->columns, sizeof(bool));
    }
    kmap->prefix_sum_matrix = newMalloc((kmap->lines + 1) * sizeof(int*));
    for (size_t i = 0; i <= kmap->lines; i++) {
        kmap->prefix_sum_matrix[i] = newCalloc(kmap->columns + 1, sizeof(int));
    }
    bool *expression_value = newMalloc(kmap->variable_count * sizeof(bool));
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
    return kmap;
}

// Frees a Kmap struct
void freeKmap(Kmap *kmap) {
    for (size_t i = 0; i < kmap->lines; i++) {
        free(kmap->matrix[i]);
        free(kmap->prefix_sum_matrix[i]);
    }
    free(kmap->prefix_sum_matrix[kmap->lines]);
    free(kmap->matrix);
    free(kmap->prefix_sum_matrix);
    free(kmap->variables);
    free(kmap->variable_map);
    free(kmap);
}

// Puts 1 at a specific cell in the Kmap matrix based on given expression bits
void fillKmapCell(Kmap *kmap, bool *expression_value) {
    const size_t line = (size_t) expression_value[0];
    const size_t column = (size_t) fromGrayCode(expression_value + 1, kmap->variable_count - 1);
    kmap->matrix[line][column] = 1;
}

// Returns an integer from a Gray Code
unsigned int fromGrayCode(bool *expression_value, size_t length) {
    unsigned int gray = 0;
    unsigned int pw = 1 << (length - 1);
    for (size_t i = 0; i < length; i++) {
        gray += pw * (unsigned int) expression_value[i];
        pw >>= 1;
    }
    unsigned int n = gray;
    while (gray >>= 1) n ^= gray;
    return n;
}

// Solves a Kmap and show the resulting sum of products expression
void solveKmap(Kmap *kmap) {
    fillPrefiSumMatrix(kmap);
    // TO DO
}

// Fills the prefix sum matrix of the kmap matrix
void fillPrefiSumMatrix(Kmap *kmap) {
    int **m = kmap->prefix_sum_matrix;
    for (size_t i = 1; i <= kmap->lines; i++) {
        for (size_t j = 1; j <= kmap->columns; j++) {
            m[i][j] = m[i - 1][j] + m[i][j - 1] - m[i - 1][j - 1] + (int) kmap->matrix[i - 1][j - 1];
        }
    }
}

// Gets the sum of a matrix based on its prefix sum matrix, from a square formed
// from the points (i0, j0) in the upper left and (i1, j1) in the lower right
int getSquareSum(int **matrix, int i0, int j0, int i1, int j1) {
    return matrix[i1 + 1][j1 + 1] + matrix[i0][j0] - matrix[i0][j1 + 1] - matrix[i1 + 1][j0];
}
