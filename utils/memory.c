#include "memory.h"

void *Malloc(size_t size, char* errorMessage) {
    void *ret = malloc(size);
    if (!ret) {
        fprintf(stderr, "%s\n", errorMessage);
        exit(1);
    }
    return ret;
}

void *Calloc(int length, size_t size, char* errorMessage) {
    void *ret = calloc(length, size);
    if (!ret) {
        fprintf(stderr, "%s\n", errorMessage);
        exit(1);
    }
    return ret;
}