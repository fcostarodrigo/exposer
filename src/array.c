#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "array.h"

#ifndef DEBUG
#define DEBUG 0
#endif

const long ARRAY_SIZE = 1024l;

int arrayInit(struct array *array, long unit) {
    return arrayInitSize(array, unit, ARRAY_SIZE);
}

int arrayInitSize(struct array *array, long unit, long capacity) {
    if (unit < 1) {
        if (DEBUG) {
            printf("array> Initialization with invalid unit.\n");
        }
        arrayEmpty(array);
        return 0;
    }

    if (capacity < 1) {
        if (DEBUG) {
            printf("array> Initialization with invalid capacity.\n");
        }
        arrayEmpty(array);
        return 0;
    }

    if (DEBUG) {
        if (capacity > 1) {
            printf("array> Creating array of %ld elements ", capacity);
        } else {
            printf("array> Creating array of %ld element ", capacity);
        }

        if (unit > 1) {
            printf("of %ld bytes.\n", unit);
        } else {
            printf("of %ld byte.\n", unit);
        }
    }

    array->base = malloc(capacity * unit);
    array->data = array->base;
    array->end = array->data;
    array->boundary = array->base + capacity * unit;
    array->unit = unit;

    if (array->base == NULL) {
        fprintf(stderr, "array> Error allocating memory.\n");
        arrayEmpty(array);
        return 0;
    }

    return 1;
}

void arrayEmpty(struct array *array) {
    array->base = NULL;
    array->data = NULL;
    array->end = NULL;
    array->boundary = NULL;
    array->unit = 0;
}

int arrayFree(struct array *array) {
    if (array-> base == NULL) {
        if (DEBUG) {
            printf("array> Removing meta data.\n");
        }
        arrayEmpty(array);
        return 0;
    }

    if (DEBUG) {
        printf("array> Removing array.\n");
    }

    free(array->base);
    arrayEmpty(array);

    return 1;
}

void arrayClean(struct array *array) {
    if (DEBUG) {
        printf("array> Removing garbage.\n");
    }

    if (array->base == array->data) {
        return;
    }

    if (array->end - array->data > 0) {
        memmove(array->base, array->data, array->end - array->data);
    }

    array->end = array->base + (array->end - array->data);
    array->data = array->base;
}

void arrayReset(struct array *array) {
    if (DEBUG) {
        printf("array> Resetting.\n");
    }

    array->data = array->base;
    array->end = array->data;
}

int arrayDouble(struct array *array) {
    arrayExtend(array, (array->boundary - array->base) * 2 / array->unit);
}

int arrayExtend(struct array *array, long capacity) {
    struct array old;

    if (DEBUG) {
        printf("array> Extending capacity to %ld.\n", capacity / array->unit);
    }

    old = array[0];
    capacity = capacity * array->unit;
    array->base = realloc(array->base, capacity);
    array->data = array->base + (old.data - old.base);
    array->end = array->base + (old.end - old.base);
    array->boundary = array->base + capacity;

    if (array->base == NULL) {
        fprintf(stderr, "array> Error allocating memory.\n");
        arrayEmpty(array);
        return 0;
    }

    return 1;
}

void arrayDump(struct array *array) {
    char *data;

    for (data = array->data; data < array->end; ++data) {
        printf("array> array[%02ld] = 0x%2.2x\n", data - array->data, data[0]);
    }
}

long arrayGarbage(struct array *array) {
    return (array->data - array->base) / array->unit;
}

long arraySize(struct array *array) {
    return (array->end - array->data) / array->unit;
}

long arrayLeft(struct array *array) {
    return (array->boundary - array->end) / array->unit;
}

int arraySetSize(struct array *array, long n) {
    n = n * array->unit;

    if (array->data + n > array->boundary) {
        if (DEBUG) {
            printf("array> Set size overflow.\n");
        }
        return 0;
    }

    array->end = array->data + n;

    if (DEBUG) {
        printf("array> Setting number of elements");
        printf(" to %ld.\n", arraySize(array));
    }

    return 1;
}

int arrayAddSize(struct array *array, long n) {
    n = n * array->unit;

    if (array->end + n > array->boundary) {
        if (DEBUG) {
            printf("array> Add size overflow.\n");
        }
        return 0;
    }

    array->end = array->end + n;

    if (DEBUG) {
        printf("array> Setting number of elements");
        printf(" to %ld.\n", arraySize(array));
    }

    return 1;
}

int arrayForget(struct array *array, long n) {
    n = n * array->unit;

    if (array->data + n > array->end) {
        if (DEBUG) {
            printf("array> Forget overflow.\n");
        }
        return 0;
    }

    if (DEBUG) {
        printf("array> Forgetting %ld elements.\n", n / array->unit);
    }

    array->data = array->data + n;
    return 1;
}

int arraySetI(struct array *array, long i, long n, char **begin, char **end) {
    if (n <= 0) {
        if (DEBUG) {
            printf("array> Invalid number of elements.\n");
        }
        return 0;
    }

    if (i < 0) {
        i = i + arraySize(array) + 1;
    }

    begin[0] = array->data + i * array->unit;
    end[0] = array->data + (i + n) * array->unit;

    if (begin[0] < array->data) {
        if (DEBUG) {
            printf("array> Set index underflow.\n");
        }
        return 0;
    }

    if (begin[0] > array->end) {
        if (DEBUG) {
            printf("array> Set index overflow.\n");
        }
        return 0;
    }

    return 1;
}

int arrayGetI(struct array *array, long i, long n, char **begin, char **end) {
    if (n <= 0) {
        if (DEBUG) {
            printf("array> Invalid number of elements.\n");
        }
        return 0;
    }

    if (i < 0) {
        i = i + arraySize(array);
    }

    begin[0] = array->data + i * array->unit;
    end[0] = array->data + (i + n) * array->unit;

    if (begin[0] < array->data) {
        if (DEBUG) {
            printf("array> Get index underflow.\n");
        }
        return 0;
    }

    if (begin[0] >= array->end) {
        if (DEBUG) {
            printf("array> Get index overflow.\n");
        }
        return 0;
    }

    if (end[0] > array->end) {
        if (DEBUG) {
            printf("array> Get index overflow.\n");
        }
        return 0;
    }

    return 1;
}

int arraySet(struct array *array, long i, void *source, long n) {
    char *begin, *end;

    if (source == NULL) {
        if (DEBUG) {
            printf("array> Set with invalid source.\n");
        }
        return 0;
    }

    if (!arraySetI(array, i, n, &begin, &end)) {
        return 0;
    }

    while (end > array->boundary) {
        arrayDouble(array);
        arraySetI(array, i, n, &begin, &end);
    }

    if (end > array->end) {
        array->end = end;

        if (DEBUG) {
            printf("array> Setting number of elements");
            printf(" to %ld.\n", arraySize(array));
        }
    }

    if (DEBUG) {
        if (n > 1) {
            printf("array> Setting %ld elements ", n);
        } else {
            printf("array> Setting %ld element ", n);
        }
        printf("at %ld.\n", i);
    }

    memcpy(begin, source, end - begin);

    return 1;
}

int arrayGet(struct array *array, long i, void *target, long n) {
    char *begin, *end;

    if (target == NULL) {
        if (DEBUG) {
            printf("array> Get with invalid target.\n");
        }
        return 0;
    }

    if (!arrayGetI(array, i, n, &begin, &end)) {
        return 0;
    }

    if (DEBUG) {
        if (n > 1) {
            printf("array> Getting %ld elements ", n);
        } else {
            printf("array> Getting %ld element ", n);
        }
        printf("at %ld.\n", i);
    }

    memcpy(target, begin, end - begin);

    return 1;
}

int arrayInsert(struct array *array, long i, void *source, long n) {
    char *begin, *end;
    long moving;

    if (source == NULL) {
        if (DEBUG) {
            printf("array> Insert with invalid source.\n");
        }
        return 0;
    }

    if (!arraySetI(array, i, n, &begin, &end)) {
        return 0;
    }

    while (end > array->boundary) {
        arrayDouble(array);
        arraySetI(array, i, n, &begin, &end);
    }

    moving = array->end - begin;

    array->end = array->end + n * array->unit;
    if (DEBUG) {
        printf("array> Setting number of elements");
        printf(" to %ld.\n", arraySize(array));
    }

    if (moving > 0) {
        memmove(end, begin, moving);

        if (DEBUG) {
            moving = moving / array->unit;
            if (moving > 1) {
                printf("array> Moving %ld elements ", moving);
            } else {
                printf("array> Moving %ld element ", moving);
            }
            printf("from %ld to %ld.\n", i, i + n);
        }
    }

    if (DEBUG) {
        if (n > 1) {
            printf("array> Setting %ld elements ", n);
        } else {
            printf("array> Setting %ld element ", n);
        }
        printf("at %ld.\n", i);
    }

    memcpy(begin, source, end - begin);

    return 1;
}

int arrayRemove(struct array *array, long i, void *target, long n) {
    char *begin, *end;
    long moving;

    if (!arrayGetI(array, i, n, &begin, &end)) {
        return 0;
    }

    if (target != NULL) {
        memcpy(target, begin, end - begin);
    }

    moving = array->end - end;
//printf("moving = %ld\n", moving);
//printf("begin = %ld\n", begin - array->base);
//printf("end = %ld\n", end - array->base);
//printf("array->end = %ld\n", array->end - array->base);
//printf("array->end - moving = %ld\n", (array->end - moving) - array->base);
//arrayDump(array);
    array->end = array->end - (end - begin);

    if (moving > 0) {
        memmove(begin, end, moving);

        if (DEBUG) {
            moving = moving / array->unit;
            if (moving > 1) {
                printf("array> Moving %ld elements ", moving);
            } else {
                printf("array> Moving %ld element ", moving);
            }
            printf("from %ld to %ld.\n", i + n, i);
        }
    }

    if (DEBUG) {
        printf("array> Setting number of elements to %ld.\n", arraySize(array));
    }

    return 1;
}

int arrayPush(struct array *array, void *source, long n) {
    return arraySet(array, -1, source, n);
}

int arrayPop(struct array *array, void *target, long n) {
    return arrayRemove(array, -1, target, n);
}

int arrayShift(struct array *array, void *target, long n) {
    return arrayRemove(array, 0, target, n);
}

int arrayUnshift(struct array *array, void *source, long n) {
    return arrayInsert(array, 0, source, n);
}
