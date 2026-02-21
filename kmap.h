#ifndef KMAP_HEADER
#define KMAP_HEADER
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    bool **matrix;
    char *variables;
    size_t variable_count;
    size_t *variable_map;
    size_t lines;
    size_t columns;
} Kmap;

Kmap *createKmap(char *expression, size_t length);
void fillKmapCell(Kmap *kmap, bool *expression_value);
void freeKmap(Kmap *kmap);
unsigned int toGrayCode(unsigned int n);
unsigned int fromGrayCode(unsigned int g);

#endif