#ifndef INPUT_HEADER
#define INPUT_HEADER
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    char *content;
    size_t size;
    size_t length;
} Buffer;

typedef struct {
    char **content;
    size_t size;
} VariableSet;

Buffer *createBuffer(size_t size);
VariableSet *createVariableSet(size_t size);
void freeBuffer(Buffer *buffer);
void freeVariableSet(VariableSet *variable_set);
void readInput(Buffer *buffer, const char *file_name);
void removeSpaces(Buffer *buffer);
void validateExpression(Buffer *buffer);
void checkValidSumExpression(Buffer *buffer);
size_t getProductCount(Buffer *buffer);
void checkValidProductExpressions(Buffer *buffer, VariableSet *variable_set);
bool isValidProductExpression(VariableSet *set, size_t line, char *string, size_t l, size_t r);
bool isValidVariableChar(char c);
void checkEqualVariablesBetweenProducts(VariableSet *variable_set);

#endif