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
    kmap->expression_count = 0;
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
    kmap->used_matrix = newMalloc(kmap->lines * sizeof(bool*));
    for (size_t i = 0; i < kmap->lines; i++) {
        kmap->matrix[i] = newCalloc(kmap->columns, sizeof(bool));
        kmap->used_matrix[i] = newCalloc(kmap->columns, sizeof(bool));
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
    Node *p = kmap->expression_head;
    while (p != NULL) {
        Node *next = p->next;
        free(p->expression);
        free(p);
        p = next;
    }
    for (size_t i = 0; i < kmap->lines; i++) {
        free(kmap->matrix[i]);
        free(kmap->used_matrix[i]);
        free(kmap->prefix_sum_matrix[i]);
    }
    free(kmap->prefix_sum_matrix[kmap->lines]);
    free(kmap->matrix);
    free(kmap->used_matrix);
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
int fromGrayCode(bool *expression_value, size_t length) {
    int gray = 0;
    int pw = 1 << (length - 1);
    for (size_t i = 0; i < length; i++) {
        gray += pw * (int) expression_value[i];
        pw >>= 1;
    }
    int n = gray;
    while (gray >>= 1) n ^= gray;
    return n;
}

// Returns a Gray Code from an integer
int toGrayCode(int n) {
    return n ^ (n >> 1);
}

// Solves a Kmap and show the resulting sum of products expression
void solveKmap(Kmap *kmap) {
    fillPrefiSumMatrix(kmap);
    int total_sum = getRectangleSum(kmap->prefix_sum_matrix, 0, 0, (int) kmap->lines - 1, (int) kmap->columns - 1);
    if (total_sum == (int) kmap->lines * kmap->columns) {
        printf("The expression is true for all cases.\n");
        return;
    }
    for (int area = (int) kmap->columns; 2 <= area; area >>= 1) {
        // One line
        for (int line = 0; line < 2; line++) {
            for (int col = 0; col <= (int) kmap->columns - area; col++) {
                int sum = getRectangleSum(kmap->prefix_sum_matrix, line, col, line, col + area - 1);
                if (sum == area && !rectangleIsFullyUsed(kmap->used_matrix, line, col, line, col + area - 1)) {
                    fillUsedRectangle(kmap->used_matrix, line, col, line, col + area - 1);
                    buildOneLineExpression(kmap, line, col, line, col + area - 1);
                }
            }
            if (area == (int) kmap->columns) continue;
            // Mirroed
            int sum = getRectangleSum(kmap->prefix_sum_matrix, line, 0, line, area / 2 - 1) +
                      getRectangleSum(kmap->prefix_sum_matrix, line, kmap->columns - area / 2, line, kmap->columns - 1);
            if (sum == area && !rectangleIsFullyUsed(kmap->used_matrix, line, 0, line, area / 2 - 1) &&
                !rectangleIsFullyUsed(kmap->used_matrix, line, kmap->columns - area / 2, line, kmap->columns - 1))
            {
                fillUsedRectangle(kmap->used_matrix, line, 0, line, area / 2 - 1);
                fillUsedRectangle(kmap->used_matrix, line, kmap->columns - area / 2, line, kmap->columns - 1);
                buildMirroedOneLineExpression(kmap, line, 0, area / 2 - 1, kmap->columns - area / 2, kmap->columns - 1);
            }
        }
        // Two lines
    }
    Node *node = kmap->expression_head;
    for (size_t i = 0; i < kmap->expression_count; i++) {
        printf("%s", node->expression);
        if (i < kmap->expression_count - 1) printf(" %c ", SUM_CHAR);
    }
    printf("\n");
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

// Gets the sum of a matrix based on its prefix sum matrix, from a rectangle formed
// from the points (i0, j0) in the upper left and (i1, j1) in the lower right
int getRectangleSum(int **matrix, int i0, int j0, int i1, int j1) {
    return matrix[i1 + 1][j1 + 1] + matrix[i0][j0] - matrix[i0][j1 + 1] - matrix[i1 + 1][j0];
}

// Checks if all the squares of the rectangle in the Kmap where already used in another group
bool rectangleIsFullyUsed(bool **matrix, int i0, int j0, int i1, int j1) {
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            if (!matrix[i][j]) return false;
        }
    }
    return true;
}

// Marks all the squares of the rectangle in the Kmap as "used"
void fillUsedRectangle(bool **matrix, int i0, int j0, int i1, int j1) {
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            matrix[i][j] = 1;
        }
    }
}

void buildOneLineExpression(Kmap *kmap, int i0, int j0, int i1, int j1) {
    bool *changed_bits = newCalloc(kmap->variable_count - 1, sizeof(bool));
    bool *pivot_bits = newMalloc((kmap->variable_count - 1) * sizeof(bool));
    int pivot_number = toGrayCode(j0);
    int cur_number = pivot_number;
    for (size_t i = 0; i < kmap->variable_count - 1; i++, cur_number >>= 1) {
        pivot_bits[kmap->variable_count - 2 - i] = (bool) (cur_number & 1);
    }
    for (int j = j0; j <= j1; j++) {
        cur_number = toGrayCode(j);
        for (size_t i = 0; i < kmap->variable_count - 1; i++, cur_number >>= 1) {
            if (pivot_bits[kmap->variable_count - 2 - i] != (bool) (cur_number & 1)) {
                changed_bits[kmap->variable_count - 2 - i] = 1;
            }
        }
    }
    buildExpressionString(kmap, changed_bits, pivot_bits, i0);
}

void buildMirroedOneLineExpression(Kmap *kmap, int line, int j0, int j1, int j2, int j3) {
    bool *changed_bits = newCalloc(kmap->variable_count - 1, sizeof(bool));
    bool *pivot_bits = newMalloc((kmap->variable_count - 1) * sizeof(bool));
    int pivot_number = toGrayCode(j0);
    int cur_number = pivot_number;
    for (size_t i = 0; i < kmap->variable_count - 1; i++, cur_number >>= 1) {
        pivot_bits[kmap->variable_count - 2 - i] = (bool) (cur_number & 1);
    }
    for (int j = j0; j <= j1; j++) {
        cur_number = toGrayCode(j);
        for (size_t i = 0; i < kmap->variable_count - 1; i++, cur_number >>= 1) {
            if (pivot_bits[kmap->variable_count - 2 - i] != (bool) (cur_number & 1)) {
                changed_bits[kmap->variable_count - 2 - i] = 1;
            }
        }
    }
    for (int j = j2; j <= j3; j++) {
        cur_number = toGrayCode(j);
        for (size_t i = 0; i < kmap->variable_count - 1; i++, cur_number >>= 1) {
            if (pivot_bits[kmap->variable_count - 2 - i] != (bool) (cur_number & 1)) {
                changed_bits[kmap->variable_count - 2 - i] = 1;
            }
        }
    }
    buildExpressionString(kmap, changed_bits, pivot_bits, line);
}

void buildExpressionString(Kmap *kmap, bool *changed_bits, bool *pivot_bits, int line) {
    size_t expression_char_count = 1;
    if (line == 0) {
        expression_char_count++;
    }
    for (size_t i = 0; i < kmap->variable_count - 1; i++) {
        if (!changed_bits[i]) {
            expression_char_count++;
            if (pivot_bits[i] == 0) expression_char_count++;
        }
    }
    char *expression = newMalloc((expression_char_count + 1) * sizeof(char));
    size_t expression_ptr = 0;
    size_t variable_ptr = 0;
    if (line == 0) {
        expression[expression_ptr++] = NEGATION_CHAR;
    }
    expression[expression_ptr++] = kmap->variables[0];
    for (size_t i = 0; i < kmap->variable_count - 1; i++) {
        if (!changed_bits[i]) {
            if (pivot_bits[i] == 0) {
                expression[expression_ptr++] = NEGATION_CHAR;
            }
            expression[expression_ptr++] = kmap->variables[i + 1];
        }
    }
    expression[expression_ptr] = '\0';
    free(changed_bits);
    free(pivot_bits);
    Node *node = newMalloc(sizeof(Node));
    node->expression = expression;
    kmap->expression_count++;
    node->next = kmap->expression_head;
    kmap->expression_head = node;
}
