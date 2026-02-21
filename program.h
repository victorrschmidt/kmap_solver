#ifndef PROGRAM_HEADER
#define PROGRAM_HEADER

// Program running
#define INPUT_FILE_NAME "notation.txt"
#define NEGATION_CHAR '!'
#define SUM_CHAR '+'
#define ACCEPTED_VARIABLE_CHARS "abcdefghijklmnopqrstuvwxyz"
#define MAX_ASCII_DECIMAL_VALUE 126
#define MAX_BUFFER_SIZE 128

// Program messages
#define DEFAULT_REPEATED_ACCEPTED_VARIABLE_CHAR_ERROR_MESSAGE "Fatal error: All accepted variable chars must be different."
#define DEFAULT_NEGATION_CHAR_AT_VARIABLE_ERROR_MESSAGE       "Fatal error: The negation char can't be an accepted variable."
#define DEFAULT_EMPTY_INPUT_ERROR_MESSAGE                     "Error: Empty input error."
#define DEFAULT_ALLOCATION_ERROR_MESSAGE                      "Error: Memory allocation error."
#define DEFAULT_SUM_SYMBOL_START_END_ERROR_MESSAGE            "Error: Invalid sum expression.\nCan't start and/or end with '+'."
#define DEFAULT_SUM_SYMBOL_EMPTY_ERROR_MESSAGE                "Error: Invalid sum expression.\nEmpty product between two '+'."
#define DEFAULT_INVALID_PRODUCT_ERROR_MESSAGE                 "Error: Invalid product expression."
#define DEFAULT_REPETEAD_VARIABLE_ERROR_MESSAGE               "Error: Invalid product expression.\nRepeated variable on product."
#define DEFAULT_VARIABLE_SET_DIFFERS_ERROR_MESSAGE            "Error: Invalid product expression.\nAll products must have the same set of variables."

void endProgram(const char *message);
void assertProgram();

#endif
