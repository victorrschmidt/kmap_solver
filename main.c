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

    // TO DO

    freeKmap(kmap);

	return 0;
}
