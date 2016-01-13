#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/file.h"

void size(void) {
    int data[] = {1, 2, 3, 4};
    FILE *file;

    file = fopen("test", "wr");

    fwrite(data, sizeof(data), 1, file);
    assert(fileSize(file) == sizeof(data));

    fclose(file);

    remove("test");
}

void buffer(void) {
    int source[] = {1, 2, 3, 4};
    int target[] = {0, 0, 0, 0};
    int n;
    FILE *file;

    file = fopen("test", "wr");

    fwrite(source, sizeof(source), 1, file);
    fileBuffer(file, &target, &n);

    assert(memcmp(source, target, sizeof(source)));

    fclose(file);

    remove("test");
}

int main(void) {

    size();
    buffer();
    return 0;
}
