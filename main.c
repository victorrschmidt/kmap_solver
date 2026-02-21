#include "program.h"
#include "input.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

int main() {
    assertProgram();
    Buffer *buffer = createBuffer(MAX_BUFFER_SIZE);

    readInput(buffer, INPUT_FILE_NAME);
    validateExpression(buffer);

    printf("Valid expression.\n");

    /*
        lines and columns

        2 -> {
            1x2
            2x1
        }
        4 -> {
            1x4
            2x2
            4x1
        }
        8 -> {
            1x8
            2x4
            4x2
            8x1
        }
        16 -> {
            1x16
            2x8
            4x4
            8x2
            16x1
        }

    */

    freeBuffer(buffer);

	return 0;
}
