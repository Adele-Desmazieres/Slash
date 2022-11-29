#ifndef JOKER_SIMPLE_H
#define JOKER_SIMPLE_H

typedef struct pathArray {
    int len;
    char** content;
} pathArray;

int containsSimpleJoker (const char* path);
void freePathArray(pathArray* arr);
int isSuffix(const char* str, const char* suffix);
char* truncatePath(const char* path);
char* getSuffix(const char* path);
pathArray* jokerSimple(const char* str);


#endif