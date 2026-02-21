#ifndef PROGRAM_HEADER
#define PROGRAM_HEADER
#define DEFAULT_EMPTY_INPUT_ERROR_MESSAGE "Error: Empty input error."
#define DEFAULT_ALLOCATION_ERROR_MESSAGE "Error: Memory allocation error."
#define DEFAULT_SUM_SYMBOL_START_END_ERROR_MESSAGE "Error: Invalid sum expression.\nCan't start and/or end with '+'."
#define DEFAULT_SUM_SYMBOL_EMPTY_ERROR_MESSAGE "Error: Invalid sum expression.\nEmpty product between two '+'."
#define DEFAULT_INVALID_PRODUCT_ERROR_MESSAGE "Error: Invalid product expression."
#define DEFAULT_REPETEAD_VARIABLE_ERROR_MESSAGE "Error: Invalid product expression.\nRepeated variable on product."
#define DEFAULT_VARIABLE_SET_DIFFERS_ERROR_MESSAGE "Error: Invalid product expression.\nAll products must have the same variables."
#define VALID_VARIABLE_COUNT 26

void endProgram(const char *message);

#endif