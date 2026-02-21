#include "program.h"
#include "input.h"
#include "kmap.h"
#include <stdio.h>

int main() {
    assertProgram();

    Buffer *buffer = createBuffer();
    readInput(buffer);

    printf("Checking expression...\n");
    validateExpression(buffer);
    printf("Expression checked.\n");

    Kmap *kmap = createKmap(buffer->content, buffer->length);
    freeBuffer(buffer);

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

    freeKmap(kmap);

	return 0;
}
