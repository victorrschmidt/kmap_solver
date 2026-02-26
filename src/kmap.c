#include "kmap.h"
#include "stringset.h"
#include "program.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Creates a Kmap struct based on given sum of products expression
Kmap *createKmap(char *expression, size_t length) {
    Kmap *kmap = newMalloc(sizeof(Kmap));
    kmap->expressions = newMalloc(sizeof(StringSet));
    initStringSet(kmap->expressions);
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

// Shows informations about the kmap
void debugKmap(Kmap *kmap) {
    printf("[KMAP]\nVariables: %s\nVariable count: %zu\n\n", kmap->variables, kmap->variable_count);
}

// Frees a Kmap struct
void freeKmap(Kmap *kmap) {
    for (size_t i = 0; i < kmap->lines; i++) {
        free(kmap->matrix[i]);
        free(kmap->used_matrix[i]);
        free(kmap->prefix_sum_matrix[i]);
    }
    freeStringSet(kmap->expressions);
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
    fillPrefixSumMatrix(kmap);
    Rectangle full_matrix = {0, 0, kmap->lines - 1, kmap->columns - 1};
    int total_sum = getRectangleSum(kmap->prefix_sum_matrix, full_matrix);
    if (total_sum == (int) kmap->lines * kmap->columns) {
        printf("The expression is true for all cases.\n");
        return;
    }
    Rectangle *rectangles = newMalloc(2 * sizeof(Rectangle));
    for (size_t area = kmap->columns; 2 <= area; area >>= 1) {
        for (size_t line = 0; line < 2; line++) {
            for (size_t col = 0; col <= kmap->columns - area; col++) {
                rectangles[0].i0 = line;
                rectangles[0].j0 = col;
                rectangles[0].i1 = line;
                rectangles[0].j1 = col + area - 1;
                int sum = getRectangleSum(kmap->prefix_sum_matrix, rectangles[0]);
                if (sum == area && !rectangleIsFullyUsed(kmap->used_matrix, rectangles[0])) {
                    fillUsedRectangle(kmap->used_matrix, rectangles[0]);
                    buildExpression(kmap, rectangles, 1);
                }
            }
            if (area == kmap->columns) continue;
            rectangles[0].i0 = line;
            rectangles[0].j0 = 0;
            rectangles[0].i1 = line;
            rectangles[0].j1 = area / 4;
            rectangles[1].i0 = line;
            rectangles[1].j0 = kmap->columns - area / 2;
            rectangles[1].i1 = line;
            rectangles[1].j1 = kmap->columns - 1;
            int sum = getRectangleSum(kmap->prefix_sum_matrix, rectangles[0]) + getRectangleSum(kmap->prefix_sum_matrix, rectangles[1]);
            if (sum == area && !rectangleIsFullyUsed(kmap->used_matrix, rectangles[0]) && !rectangleIsFullyUsed(kmap->used_matrix, rectangles[1])) {
                fillUsedRectangle(kmap->used_matrix, rectangles[0]);
                fillUsedRectangle(kmap->used_matrix, rectangles[1]);
                buildExpression(kmap, rectangles, 2);
            }
        }
        if (area == kmap->columns) {
            rectangles[0].i0 = 0;
            rectangles[0].j0 = 0;
            rectangles[0].i1 = 1;
            rectangles[0].j1 = area / 2 - 1;
            int sum = getRectangleSum(kmap->prefix_sum_matrix, rectangles[0]);
            if (sum == area && !rectangleIsFullyUsed(kmap->used_matrix, rectangles[0])) {
                fillUsedRectangle(kmap->used_matrix, rectangles[0]);
                buildExpression(kmap, rectangles, 1);
            }
            rectangles[0].i0 = 0;
            rectangles[0].j0 = area / 2;
            rectangles[0].i1 = 1;
            rectangles[0].j1 = area - 1;
            sum = getRectangleSum(kmap->prefix_sum_matrix, rectangles[0]);
            if (sum == area && !rectangleIsFullyUsed(kmap->used_matrix, rectangles[0])) {
                fillUsedRectangle(kmap->used_matrix, rectangles[0]);
                buildExpression(kmap, rectangles, 1);
            }
            continue;
        }
        for (size_t col = 0; col <= kmap->columns - area / 2; col++) {
            rectangles[0].i0 = 0;
            rectangles[0].j0 = col;
            rectangles[0].i1 = 1;
            rectangles[0].j1 = col + area / 2 - 1;
            int sum = getRectangleSum(kmap->prefix_sum_matrix, rectangles[0]);
            if (sum == area && !rectangleIsFullyUsed(kmap->used_matrix, rectangles[0])) {
                fillUsedRectangle(kmap->used_matrix, rectangles[0]);
                buildExpression(kmap, rectangles, 1);
            }
        }
        if (area < 4) continue;
        rectangles[0].i0 = 0;
        rectangles[0].j0 = 0;
        rectangles[0].i1 = 1;
        rectangles[0].j1 = area / 4 - 1;
        rectangles[1].i0 = 0;
        rectangles[1].j0 = kmap->columns - area / 4;
        rectangles[1].i1 = 1;
        rectangles[1].j1 = kmap->columns - 1;
        int sum = getRectangleSum(kmap->prefix_sum_matrix, rectangles[0]) + getRectangleSum(kmap->prefix_sum_matrix, rectangles[1]);
        if (sum == area && !rectangleIsFullyUsed(kmap->used_matrix, rectangles[0]) && !rectangleIsFullyUsed(kmap->used_matrix, rectangles[1])) {
            fillUsedRectangle(kmap->used_matrix, rectangles[0]);
            fillUsedRectangle(kmap->used_matrix, rectangles[1]);
            buildExpression(kmap, rectangles, 2);
        }
    }
    for (size_t i = 0; i < kmap->lines; i++) {
        for (size_t j = 0; j < kmap->columns; j++) {
            if (kmap->matrix[i][j] == 1 && !kmap->used_matrix[i][j]) {
                rectangles[0].i0 = i;
                rectangles[0].j0 = j;
                rectangles[0].i1 = i;
                rectangles[0].j1 = j;
                buildExpression(kmap, rectangles, 1);
            }
        }
    }
    free(rectangles);
    showFinalExpression(kmap);
}

// Fills the prefix sum matrix of the Kmap matrix
void fillPrefixSumMatrix(Kmap *kmap) {
    int **m = kmap->prefix_sum_matrix;
    for (size_t i = 1; i <= kmap->lines; i++) {
        for (size_t j = 1; j <= kmap->columns; j++) {
            m[i][j] = m[i - 1][j] + m[i][j - 1] - m[i - 1][j - 1] + (int) kmap->matrix[i - 1][j - 1];
        }
    }
}

// Gets the sum of a matrix based on its prefix sum matrix, from a rectangle formed
// from the points (i0, j0) in the upper left and (i1, j1) in the lower right
int getRectangleSum(int **prefix_sum_matrix, Rectangle rectangle) {
    int **m = prefix_sum_matrix;
    size_t i0 = rectangle.i0;
    size_t j0 = rectangle.j0;
    size_t i1 = rectangle.i1;
    size_t j1 = rectangle.j1;
    return m[i1 + 1][j1 + 1] + m[i0][j0] - m[i0][j1 + 1] - m[i1 + 1][j0];
}

// Checks if all the cells of the rectangle in the Kmap where already used in another group
bool rectangleIsFullyUsed(bool **used_matrix, Rectangle rectangle) {
    size_t i0 = rectangle.i0;
    size_t j0 = rectangle.j0;
    size_t i1 = rectangle.i1;
    size_t j1 = rectangle.j1;
    for (size_t i = i0; i <= i1; i++) {
        for (size_t j = j0; j <= j1; j++) {
            if (used_matrix[i][j] == 0) return false;
        }
    }
    return true;
}

// Marks all the cells of the rectangle in the Kmap as "used"
void fillUsedRectangle(bool **used_matrix, Rectangle rectangle) {
    size_t i0 = rectangle.i0;
    size_t j0 = rectangle.j0;
    size_t i1 = rectangle.i1;
    size_t j1 = rectangle.j1;
    for (size_t i = i0; i <= i1; i++) {
        for (size_t j = j0; j <= j1; j++) {
            used_matrix[i][j] = 1;
        }
    }
}

// Creates a product expression based on the rectangles used as reference in the kmap
void buildExpression(Kmap *kmap, Rectangle *rectangle_list, size_t size) {
    bool *changed_bits = newCalloc(kmap->variable_count, sizeof(bool));
    bool *pivot_bits = newCalloc(kmap->variable_count, sizeof(bool));
    if (size == 1) {
        size_t i0 = rectangle_list[0].i0;
        size_t j0 = rectangle_list[0].j0;
        size_t i1 = rectangle_list[0].i1;
        size_t j1 = rectangle_list[0].j1;
        int pivot_number = toGrayCode((int) j0);
        int cur_number = pivot_number;
        for (size_t i = 0; i < kmap->variable_count - 1; i++, cur_number >>= 1) {
            pivot_bits[kmap->variable_count - 1 - i] = (bool) (cur_number & 1);
        }
        for (size_t i = i0; i <= i1; i++) {
            for (size_t j = j0; j <= j1; j++) {
                cur_number = toGrayCode(j);
                for (size_t i = 0; i < kmap->variable_count - 1; i++, cur_number >>= 1) {
                    if (pivot_bits[kmap->variable_count - 1 - i] != (bool) (cur_number & 1)) {
                        changed_bits[kmap->variable_count - 1 - i] = 1;
                    }
                }
            }
        }
        if (i0 != i1) changed_bits[0] = 1;
        else pivot_bits[0] = i0;
    }
    else {
        int pivot_number = toGrayCode((int) rectangle_list[0].j0);
        int cur_number = pivot_number;
        for (size_t i = 0; i < kmap->variable_count - 1; i++, cur_number >>= 1) {
            pivot_bits[kmap->variable_count - 1 - i] = (bool) (cur_number & 1);
        }
        for (size_t k = 0; k < 2; k++) {
            for (size_t i = rectangle_list[k].i0; i <= rectangle_list[k].i1; i++) {
                for (size_t j = rectangle_list[k].j0; j <= rectangle_list[k].j1; j++) {
                    cur_number = toGrayCode(j);
                    for (size_t i = 0; i < kmap->variable_count - 1; i++, cur_number >>= 1) {
                        if (pivot_bits[kmap->variable_count - 1 - i] != (bool) (cur_number & 1)) {
                            changed_bits[kmap->variable_count - 1 - i] = 1;
                        }
                    }
                }
            }
        }
        if (rectangle_list[0].i0 != rectangle_list[0].i1) changed_bits[0] = 1;
        else pivot_bits[0] = rectangle_list[0].i0;
    }
    size_t expression_char_count = kmap->variable_count;
    for (size_t i = 0; i < kmap->variable_count; i++) {
        if (changed_bits[i] == 0 && pivot_bits[i] == 0) expression_char_count++;
    }
    char *expression = newMalloc((expression_char_count + 1) * sizeof(char));
    size_t expression_ptr = 0;
    size_t variable_ptr = 0;
    for (size_t i = 0; i < kmap->variable_count; i++) {
        if (changed_bits[i] == 0) {
            if (pivot_bits[i] == 0) expression[expression_ptr++] = NEGATION_CHAR;
            expression[expression_ptr++] = kmap->variables[i];
        }
    }
    expression = newRealloc(expression, (expression_ptr + 1) * sizeof(char));
    expression[expression_ptr] = '\0';
    free(changed_bits);
    free(pivot_bits);
    insertStringSet(kmap->expressions, expression);
}

// Shows the optimal expression after simplifying with kmap
void showFinalExpression(Kmap *kmap) {
    bool flag = 1;
    for (size_t i = 0; i < HASH_TABLE_SIZE; i++) {
        Node *node = kmap->expressions->buckets[i];
        while (node != NULL) {
            if (!flag) printf(" %c ", SUM_CHAR);
            flag = 0;
            printf("%s", node->string);
            node = node->next;
        }
    }
    printf("\n");
}
