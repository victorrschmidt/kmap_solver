#ifndef KMAP_HEADER
#define KMAP_HEADER
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    bool **matrix;
    int **prefix_sum_matrix;
    char *variables;
    size_t *variable_map;
    size_t variable_count;
    size_t lines;
    size_t columns;
} Kmap;

Kmap *createKmap(char *expression, size_t length);
void freeKmap(Kmap *kmap);
void fillKmapCell(Kmap *kmap, bool *expression_value);
unsigned int fromGrayCode(bool *expression_value, size_t length);
void solveKmap(Kmap *kmap);
void fillPrefiSumMatrix(Kmap *kmap);
int getSquareSum(int **matrix, int i0, int j0, int i1, int j1);

#endif
