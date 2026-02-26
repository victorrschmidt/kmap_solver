#include "stringset.h"
#include "program.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void initStringSet(StringSet *string_set) {
    for (size_t i = 0; i < HASH_TABLE_SIZE; i++) {
        string_set->buckets[i] = NULL;
    }
}

void freeStringSet(StringSet *string_set) {
    for (size_t i = 0; i < HASH_TABLE_SIZE; i++) {
        Node *node = string_set->buckets[i];
        while (node != NULL) {
            Node *next = node->next;
            free(node->string);
            free(node);
            node = next;
        }
    }
    free(string_set);
}

unsigned int hashNumber(const char *string) {
    unsigned int hash = 5381;
    for (size_t i = 0; string[i] != '\0'; i++) {
        int c = (int) string[i];
        hash = ((hash << 5) + hash) + c;
    }
    return hash % HASH_TABLE_SIZE;
}

bool containsStringSet(StringSet *string_set, const char *string) {
    unsigned int index = hashNumber(string);
    Node *current = string_set->buckets[index];
    while (current != NULL) {
        if (strcmp(current->string, string) == 0) return true;
        current = current->next;
    }
    return false;
}

void insertStringSet(StringSet *string_set, const char *string) {
    if (containsStringSet(string_set, string)) return;
    unsigned int index = hashNumber(string);
    Node *node = newCalloc(1, sizeof(Node));
    size_t length = strlen(string);
    node->string = newMalloc((length + 1) * sizeof(char));
    strcpy(node->string, string);
    node->next = string_set->buckets[index];
    string_set->buckets[index] = node;
}
