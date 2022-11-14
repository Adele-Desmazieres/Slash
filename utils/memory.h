#include <stdlib.h>
#include <stdio.h>

#ifndef MEMORY_H
#define MEMORY_H 1

void* Malloc(size_t size, char* errorMessage);
void *Calloc(int length, size_t size, char* errorMessage);

#endif