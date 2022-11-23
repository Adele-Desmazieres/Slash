#ifndef JOKER_SIMPLE_H
#define JOKER_SIMPLE_H

int isSuffix(const char* str, const char* suffix);
char* truncatePath(const char* path);
char* getSuffix(const char* path);
char** jokerSimple(const char* str);


#endif