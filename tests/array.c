#include <stdio.h>
#include <assert.h>
#include "../src/array.h"

int main(void) {
    struct array array;
    int source[4] = {1, 2, 3, 4};
    int target[4] = {0, 0, 0, 0};
    int target2[4] = {0, 0, 0, 0};

    printf("array test> Testing array creation and removal.\n");
    assert(arrayInit(&array, 2) == 1);
    assert((array.boundary - array.base) / array.unit == ARRAY_SIZE);
    assert(arraySize(&array) == 0);
    assert(array.unit == 2);
    assert(arrayLeft(&array) == ARRAY_SIZE);
    assert(arrayGarbage(&array) == 0);
    assert(arrayFree(&array) == 1);

    assert(arrayInitSize(&array, 3, 128) == 1);
    assert((array.boundary - array.base) / array.unit == 128);
    assert(arraySize(&array) == 0);
    assert(array.unit == 3);
    assert(arrayLeft(&array) == 128);
    assert(arrayGarbage(&array) == 0);
    assert(arrayFree(&array) == 1);

    printf("array test> Testing array set.\n");
    assert(arrayInitSize(&array, sizeof(int), 3) == 1);

    printf("array test> Testing array set underflow.\n");
    assert(arraySet(&array, -5, source, 1) == 0);

    printf("array test> Testing array set overflow.\n");
    assert(arraySet(&array, 1, source, 1) == 0);

    printf("array test> Testing array set and get to the end.\n");
    assert(arraySet(&array, 0, source, 1) == 1);
    assert(arrayGet(&array, 0, target, 1) == 1);
    assert(source[0] == target[0]);
    assert((array.boundary - array.base) / array.unit == 3);
    assert(arraySize(&array) == 1);
    assert(array.unit == sizeof(int));
    assert(arrayLeft(&array) == 2);
    assert(arrayGarbage(&array) == 0);
    assert(arrayFree(&array) == 1);
    target[0] = 0;

    printf("array test> Testing array get.\n");
    assert(arrayInitSize(&array, sizeof(int), 3) == 1);

    printf("array test> Testing array get underflow.\n");
    assert(arrayGet(&array, -5, source, 1) == 0);

    printf("array test> Testing array get overflow.\n");
    assert(arrayGet(&array, 0, source, 1) == 0);
    assert(arrayFree(&array) == 1);

    printf("array test> Testing array insert.\n");
    assert(arrayInitSize(&array, sizeof(int), 4) == 1);

    printf("array test> Testing array insert in the middle.\n");
    assert(arrayInsert(&array, 0, source, 1) == 1);
    assert((array.boundary - array.base) / array.unit == 4);
    assert(arraySize(&array) == 1);
    assert(array.unit == sizeof(int));
    assert(arrayLeft(&array) == 3);
    assert(arrayGarbage(&array) == 0);
    target[0] = 1;

    assert(arrayInsert(&array, 0, &source[1], 1) == 1);
    assert((array.boundary - array.base) / array.unit == 4);
    assert(arraySize(&array) == 2);
    assert(array.unit == sizeof(int));
    assert(arrayLeft(&array) == 2);
    assert(arrayGarbage(&array) == 0);
    target[0] = 2;
    target[1] = 1;

    assert(arrayInsert(&array, 1, &source[2], 2) == 1);
    assert((array.boundary - array.base) / array.unit == 4);
    assert(arraySize(&array) == 4);
    assert(array.unit == sizeof(int));
    assert(arrayLeft(&array) == 0);
    assert(arrayGarbage(&array) == 0);
    target[0] = 2;
    target[1] = 3;
    target[2] = 4;
    target[3] = 1;

    assert(arrayGet(&array, 0, target2, 4) == 1);
    assert(target[0] == target2[0]);
    assert(target[1] == target2[1]);
    assert(target[2] == target2[2]);
    assert(target[3] == target2[3]);

    printf("array test> Testing array insert overflow.\n");
    assert(arrayInsert(&array, -1, source, 1) == 1);
    assert((array.boundary - array.base) / array.unit == 4 * 2);

    printf("array test> Testing array insert underflow.\n");
    assert(arrayInsert(&array, -7, source, 1) == 0);
    assert(arrayFree(&array) == 1);
    target[0] = 0;
    target[1] = 0;
    target[2] = 0;
    target[3] = 0;
    target2[0] = 0;
    target2[1] = 0;
    target2[2] = 0;
    target2[3] = 0;

    printf("array test> Testing array remove.\n");
    assert(arrayInitSize(&array, sizeof(int), 4) == 1);

    printf("array test> Testing array remove in the middle.\n");
    assert(arraySet(&array, 0, source, 4) == 1);
    assert(arrayRemove(&array, 1, target, 2) == 1);
    assert(target[0] == source[1]);
    assert(target[1] == source[2]);
    assert(arrayGet(&array, 0, target, 2) == 1);
    assert(target[0] == source[0]);
    assert(target[1] == source[3]);
    assert((array.boundary - array.base) / array.unit == 4);
    assert(arraySize(&array) == 2);
    assert(array.unit == sizeof(int));
    assert(arrayLeft(&array) == 2);
    assert(arrayGarbage(&array) == 0);

    printf("array test> Testing array remove overflow.\n");
    assert(arrayRemove(&array, 2, target, 1) == 0);

    printf("array test> Testing array remove underflow.\n");
    assert(arrayRemove(&array, -5, target, 1) == 0);
    assert(arrayFree(&array) == 1);
    target[0] = 0;
    target[1] = 0;
    target[2] = 0;
    target[3] = 0;

    printf("array test> Testing array forget.\n");
    assert(arrayInitSize(&array, sizeof(int), 4) == 1);
    assert(arraySet(&array, 0, source, 4) == 1);
    assert(arrayGet(&array, 0, target, 2) == 1);
    assert(arrayForget(&array, 2) == 1);
    assert(target[0] == source[0]);
    assert(target[1] == source[1]);
    assert(arrayGet(&array, 0, target, 2) == 1);
    assert(target[0] == source[2]);
    assert(target[1] == source[3]);
    assert((array.boundary - array.base) / array.unit == 4);
    assert(arraySize(&array) == 2);
    assert(array.unit == sizeof(int));
    assert(arrayLeft(&array) == 0);
    assert(arrayGarbage(&array) == 2);

    printf("array test> Testing array forget overflow.\n");
    assert(arrayForget(&array, 3) == 0);
    assert(arrayFree(&array) == 1);
    target[0] = 0;
    target[1] = 0;
    target[2] = 0;
    target[3] = 0;

    printf("array test> Testing array memory management.\n");
    assert(arrayInitSize(&array, sizeof(int), 4) == 1);

    printf("array test> Testing array clean.\n");
    assert(arraySet(&array, 0, source, 4) == 1);
    assert(arrayForget(&array, 2) == 1);
    assert((array.boundary - array.base) / array.unit == 4);
    assert(arraySize(&array) == 2);
    assert(array.unit == sizeof(int));
    assert(arrayLeft(&array) == 0);
    assert(arrayGarbage(&array) == 2);
    arrayClean(&array);
    assert((array.boundary - array.base) / array.unit == 4);
printf("arraySize(&array) = %ld\n", arraySize(&array));
    assert(arraySize(&array) == 2);
    assert(array.unit == sizeof(int));
    assert(arrayLeft(&array) == 2);
    assert(arrayGarbage(&array) == 0);
    assert(arrayGet(&array, 0, target, 2) == 1);
    assert(target[0] == source[2]);
    assert(target[1] == source[3]);

    printf("array test> Testing array expand.\n");
    assert(arrayDouble(&array) == 1);
    assert(arraySet(&array, 2, source, 4) == 1);
    assert((array.boundary - array.base) / array.unit == 8);
    assert(arraySize(&array) == 6);
    assert(array.unit == sizeof(int));
    assert(arrayLeft(&array) == 2);
    assert(arrayGarbage(&array) == 0);

    printf("array test> Testing array reset.\n");
    arrayReset(&array);
    assert((array.boundary - array.base) / array.unit == 8);
    assert(arraySize(&array) == 0);
    assert(array.unit == sizeof(int));
    assert(arrayLeft(&array) == 8);
    assert(arrayGarbage(&array) == 0);
    assert(arrayFree(&array) == 1);

    return 0;
}
