#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/attributes.h"

int main(void) {
    int i;

    printf("attributes test> Testing getting index of attribute.\n");
    assert(attributesIndex("accept") == ATTRIBUTES_ACCEPT);
    assert(attributesIndex("warning") == ATTRIBUTES_WARNING);

    printf("attributes test> Testing getting attribute by index.\n");
    assert(strcmp(attributes[ATTRIBUTES_ACCEPT], "accept") == 0);
    assert(strcmp(attributes[ATTRIBUTES_WARNING], "warning") == 0);

    printf("attributes test> Checking if attributes are sorted.\n");
    for (i = 0; i + 1 < ATTRIBUTES_N; ++i) {
        assert(strcmp(attributes[i], attributes[i + 1]) < 0);
    }

    printf("attributes test> Testing unknown attribute.\n");
    assert(attributesIndex("a") == -1);

    return 0;
}
