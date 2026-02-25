#ifndef KMAP_HEADER
#define KMAP_HEADER
#include <stddef.h>
#include <stdbool.h>

typedef struct Node {
    struct Node *next;
    char *expression;
} Node;

typedef struct {
    bool **matrix;
    bool **used_matrix;
    int **prefix_sum_matrix;
    char *variables;
    size_t *variable_map;
    Node *expression_head;
    size_t expression_count;
    size_t variable_count;
    size_t lines;
    size_t columns;
} Kmap;

Kmap *createKmap(char *expression, size_t length);
void freeKmap(Kmap *kmap);
void fillKmapCell(Kmap *kmap, bool *expression_value);
int fromGrayCode(bool *expression_value, size_t length);
int toGrayCode(int n);
void solveKmap(Kmap *kmap);
void fillPrefiSumMatrix(Kmap *kmap);
int getRectangleSum(int **matrix, int i0, int j0, int i1, int j1);
bool rectangleIsFullyUsed(bool **used_matrix, int i0, int j0, int i1, int j1);
void fillUsedRectangle(bool **used_matrix, int i0, int j0, int i1, int j1);
void buildOneLineExpression(Kmap *kmap, int i0, int j0, int i1, int j1);
void buildMirroedOneLineExpression(Kmap *kmap, int line, int j0, int j1, int j2, int j3);
void buildExpressionString(Kmap *kmap, bool *changed_bits, bool *pivot_bits, int line);

#endif
