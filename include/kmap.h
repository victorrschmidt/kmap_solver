#ifndef KMAP_HEADER
#define KMAP_HEADER
#include "stringset.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    size_t i0;
    size_t j0;
    size_t i1;
    size_t j1;
} Rectangle;

typedef struct {
    StringSet *expressions;
    bool **matrix;
    bool **used_matrix;
    int **prefix_sum_matrix;
    char *variables;
    size_t *variable_map;
    size_t variable_count;
    size_t lines;
    size_t columns;
} Kmap;

Kmap *createKmap(char *expression, size_t length);
void debugKmap(Kmap *kmap);
void freeKmap(Kmap *kmap);
void fillKmapCell(Kmap *kmap, bool *expression_value);
int fromGrayCode(bool *expression_value, size_t length);
int toGrayCode(int n);
void solveKmap(Kmap *kmap);
void fillPrefixSumMatrix(Kmap *kmap);
int getRectangleSum(int **prefix_sum_matrix, Rectangle rectangle);
bool rectangleIsFullyUsed(bool **used_matrix, Rectangle rectangle);
void fillUsedRectangle(bool **used_matrix, Rectangle rectangle);
void buildExpression(Kmap *kmap, Rectangle *rectangle_list, size_t size);
void showFinalExpression(Kmap *kmap);

#endif
