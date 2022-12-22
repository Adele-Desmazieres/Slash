#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"

#ifndef STRING_ARRAY_H
#define STRING_ARRAY_H

#define TRUE 1
#define FALSE 0

/// @brief The stringArr structure represent an array of string. \n
/// This structure contain a field stringArr (type \b char**) that represents the array of strings \n
/// and a \b size field (type : int) that represents the size of the current array of strings.
typedef struct stringArr {
    char** stringArr;
    int size;
} stringArr;

/* *** UTILS *** */
char* trim(char* toTrim);
int sizeOfFirstWord(const char* c, char delimiter);
/* ************* */
stringArr* createStringArray();
void freeArrayOfString(char** array, int size);
void SA_free(stringArr* array);
void SA_add(stringArr* array, char* string);
void SA_addFinalNull(stringArr* array);
void SA_remove(stringArr* array, int amount);
void SA_print(stringArr* array);
stringArr* SA_splice(stringArr* toSplice, int begin, int amount);
stringArr** SA_split(stringArr* toSplit, char* separator, int* sizeBuff);
int SA_indexOf(stringArr* base, char* toSearch);
int SA_indexOfArray(stringArr* base, stringArr* toSearch);
stringArr* SA_parseString(char* toParse);
stringArr* SA_parseArray(char** toParse, int size);
stringArr* SA_parseStringWithQuotes(char* toParse);
#endif