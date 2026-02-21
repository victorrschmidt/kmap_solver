#include "program.h"
#include <stdio.h>
#include <stdlib.h>

void endProgram(const char *message) {
    printf("%s\n", message);
    exit(1);
}
