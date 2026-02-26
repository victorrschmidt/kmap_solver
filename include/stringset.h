#ifndef STRINGSET_HEADER
#define STRINGSET_HEADER
#define HASH_TABLE_SIZE 300
#include <stdbool.h>

typedef struct Node {
    struct Node *next;
    char *string;
} Node;

typedef struct {
    Node *buckets[HASH_TABLE_SIZE];
} StringSet;

void initStringSet(StringSet *string_set);
void freeStringSet(StringSet *string_set);
unsigned int hashNumber(const char *string);
bool containsStringSet(StringSet *string_set, const char *string);
void insertStringSet(StringSet *string_set, const char *string);

#endif
