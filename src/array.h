/*
    Array

    Low level data structure that exposes the internal array of bytes to the
    user. This allows the data structure to be used with functions that expect
    an array of bytes like fwrite and fread.

    The function forget allows to lazily shift elements of the array creating
    garbage instead of moving data around or breaking strings in the middle.
*/

#ifndef ARRAY_H
#define ARRAY_H

extern const long ARRAY_SIZE;

struct array {
    char *base;
    char *data;
    char *end;
    char *boundary;
    long unit;
};

int arrayInit(struct array *array, long unit);
int arrayInitSize(struct array *array, long unit, long capacity);
void arrayEmpty(struct array *array);
int arrayFree(struct array *array);
void arrayClean(struct array *array);
void arrayReset(struct array *array);
int arrayDouble(struct array *array);
int arrayExtend(struct array *array, long capacity);
void arrayDump(struct array *array);

long arrayGarbage(struct array *array);
long arraySize(struct array *array);
long arrayLeft(struct array *array);

int arraySetSize(struct array *array, long n);
int arrayAddSize(struct array *array, long n);
int arrayForget(struct array *array, long n);
int arraySetI(struct array *array, long i, long n, char **begin, char **end);
int arrayGetI(struct array *array, long i, long n, char **b, char **end);

int arraySet(struct array *array, long i, void *source, long n);
int arrayGet(struct array *array, long i, void *target, long n);
int arrayCopy(struct array *array, long source, long target, long n);
int arrayInsert(struct array *array, long i, void *source, long n);
int arrayRemove(struct array *array, long i, void *target, long n);
int arrayPush(struct array *array, void *source, long n);
int arrayPop(struct array *array, void *target, long n);
int arrayShift(struct array *array, void *target, long n);
int arrayShiftLazy(struct array *array, void *target, long n);
int arrayUnshift(struct array *array, void *source, long n);
int arrayUnshiftLazy(struct array *array, void *source, long n);

#endif
