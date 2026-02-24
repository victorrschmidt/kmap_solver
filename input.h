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

Buffer *createBuffer();
VariableSet *createVariableSet(size_t size);
void freeBuffer(Buffer *buffer);
void freeVariableSet(VariableSet *variable_set);
void readInput(Buffer *buffer);
void removeSpaces(Buffer *buffer);
void validateExpression(Buffer *buffer);
void checkValidSumExpression(Buffer *buffer);
size_t getProductCount(Buffer *buffer);
void checkAllValidProductExpressions(Buffer *buffer, VariableSet *variable_set);
void checkValidProductExpression(VariableSet *set, size_t id, char *string, size_t l, size_t r);
bool isValidVariableChar(char c);
void checkEqualVariablesBetweenProducts(VariableSet *variable_set);

#endif
