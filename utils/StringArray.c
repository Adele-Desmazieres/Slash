#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "StringArray.h"

/* *** UTILS *** */

/// @brief Delete all spaces at the beginning and at the end of a string.
/// @param toTrim The string where we want to remove leading and trailing spaces.
/// @return The string with leading and trailing spaces removed
char* trim(char* toTrim) {
    int size = strlen(toTrim);

    while (toTrim[size-1] == ' ') {
        toTrim[size-1] = '\0';
        size -= 1;
    }

    while (toTrim[0] == ' ') { toTrim++; }

    return toTrim;
}

/// @brief Get the size of the first word of a string. A word is delimited by a space character.
/// @param c The string where we want to count the size of the first word.
/// @return The size of the first word.
int sizeOfFirstWord(const char* c) {
    int size = 0;
    while(*c != '\0' && *c != ' '){
        size++; c++;
    }
    return size;
}
/* ************* */

/// @brief Create a new instance of stringArr struct.
/// @return An instance of stringArr struct.
stringArr* createStringArray() {
    stringArr* newArray = Malloc(sizeof(stringArr), "SA creation failed.");
    newArray->size = 0;
    newArray->stringArr = Malloc(sizeof(char*)*0, "SA list creation failed.");

    return newArray;
}

/// @brief Free the memory allocated to the \b char** array of strings. 
/// @param array The array of strings that will be free'd.
/// @param size The current size of the array of strings.
void freeArrayOfString(char** array, int size) {
    for (int i = 0; i < size; i++) {
        if (array[i] == NULL) continue;
        free(array[i]);
    }
    free(array);
}

/// @brief Free the memory allocated to the \b stringArr String-Array.
/// @param array The String-Array that will be free'd.
void SA_free(stringArr* array) {
    for (int i = 0; i < array->size; i++) {
        if (array->stringArr[i] == NULL) continue;
        free(array->stringArr[i]);
    }
    free(array->stringArr);
    free(array);
}

/// @brief Add a string at the END of the String-Array. If this string is NULL, this function \n 
/// won't do anything. If you want to add a NULL pointer, please see @ref addFinalNull.
/// @param array The string array that will host the string.
/// @param string The string that will be added at the end of the String-Array.
void SA_add(stringArr* array, char* string) {
    if (string == NULL) return;
    char** newArray = realloc(array->stringArr, sizeof(char*)*(array->size+1));
    
    char* newString = Malloc(sizeof(char)*strlen(string)+1, "Malloc failed when adding a word to SA.");
    strcpy(newString, string);

    newArray[array->size] = newString;

    array->stringArr = newArray;
    array->size = array->size+1;
}

/// @brief Add a NULL pointer at the END of the String-Array. \n
/// This function can be useful for execv functions. \n   
/// If you want to add a string, please see @ref addToStringArray.
/// @param array The string array that will host the NULL pointer exception.
void SA_addFinalNull(stringArr* array) {
    char** newArray = realloc(array->stringArr, sizeof(char*)*(array->size+1));

    newArray[array->size] = NULL;

    array->stringArr = newArray;
    array->size = array->size+1;
}

/// @brief Remove n last elements from the String-Array. \n 
/// - If the specified amount is lower than 1, this function won't do anything. \n 
/// - If the specified amount is higher than String-Array size, amount will be modified to this size.
/// @param array The string array from which we will delete elements.
/// @param amount The amount of elements this function will remove.
void SA_remove(stringArr* array, int amount) {
    if (amount < 1) return;
    if (amount > array->size) amount = array->size;
    
    int newSize = array->size - amount;
    char** newArray = Malloc(sizeof(char*)*(newSize), "Failed to malloc the newArray in SA_remove.");

    for (int i = 0; i < newSize; i++) {
        char* string = array->stringArr[i];
        char* newString = Malloc(sizeof(char)*strlen(string)+1, "Failed to malloc the word in SA_remove.");
        strcpy(newString, string);

        newArray[i] = newString;
    }
    freeArrayOfString(array->stringArr, array->size);
    array->stringArr = newArray;
    array->size = newSize;
}

/// @brief Print all strings in the String-Array preceded by their index. 
/// If an element is a NULL pointer, this function will print "NULL" instead of the string value.
/// @param array The String-Array to display.
void SA_print(stringArr* array) {
    if(array->size <= 0) { printf("this String-Array is empty !\n"); return; }

    for (int i = 0; i < array->size; i++) {
        if (array->stringArr[i] == NULL) {
            printf("%d - NULL\n", i);
        }else {
            printf("%d - %s\n", i, array->stringArr[i]);
        }
    }
}

/// @brief Remove n last elements from the String-Array starting from the begin index. \n 
/// - If the specified amount is lower than 1, this function won't do anything. \n 
/// - If the specified begin index is lower than 1, begin will be modified to 0. \n 
/// - If the specified begin is higher than String-Array size, this function won't do anything. \n 
/// - If the specified amount is higher than String-Array size, amount will be modified to this size. \n 
/// @param toSplice The string array from which we will delete elements.
/// @param begin The index from which the function will start deleting the elements.
/// @param amount The amount of elements this function should remove starting from the begin index.
/// @return A new stringArr* with all deleted elements.
stringArr* SA_splice(stringArr* toSplice, int begin, int amount) {
    if (amount < 0) return createStringArray();
    if (begin < 0) begin = 0;
    if (begin >= toSplice->size) return createStringArray();
    if (amount > toSplice->size) amount = toSplice->size;

    int endIndex = begin+amount-1;
    int range = endIndex - begin;
    int newArrMaxSize = toSplice->size-range+1;
    char** newArr = Malloc(sizeof(char*) * (newArrMaxSize), "Failed to malloc the newArray in SA_remove.");

    stringArr* deleted = createStringArray();

    int newArrayRealSize = 0;
    for (int i = 0; i < toSplice->size; i++) {
        if (i >= begin && i <= endIndex) { SA_add(deleted, toSplice->stringArr[i]);  continue; }
        char* string = toSplice->stringArr[i];
        char* newString = Malloc(sizeof(char)*strlen(string)+1, "Failed to malloc the word in SA_remove.");
        strcpy(newString, string);

        newArr[newArrayRealSize] = newString;
        newArrayRealSize++;
    }

    freeArrayOfString(toSplice->stringArr, toSplice->size);
    toSplice->size = newArrayRealSize;
    toSplice->stringArr = newArr;

    return deleted;
}

/// @brief Split the String-Array to a list of childs String-Array based on the specified separator. \n 
/// The initial String-Array WON'T be affected. \n 
/// Note that if there is two separators that follow each other, this function will generate an empty
/// String-Array Child.
/// @param toSplit The String-Array to split.
/// @param separator The separator that will delimit each String-Array children. 
/// Separators won't be added to any child.
/// @param sizeBuff A buffer that will contain, after the execution of the function.
/// The amount of children generated.
/// @return 
stringArr** SA_split(stringArr* toSplit, char* separator, int* sizeBuff) {
    stringArr** splitted = Malloc(sizeof(stringArr*), "Failed to malloc the new SA in SA_split");
    *sizeBuff = 1;
    splitted[0] = createStringArray();

    for (int i = 0; i < toSplit->size; i++) {
        if (strcmp(separator, toSplit->stringArr[i]) == 0) {
            *sizeBuff += 1;
            stringArr** newSplit = realloc(splitted, sizeof(stringArr*)*(*sizeBuff));
            newSplit[*sizeBuff-1] = createStringArray();
            splitted = newSplit;
        }else {
            SA_add(splitted[*sizeBuff-1], toSplit->stringArr[i]);
        }
    }
    
    return splitted;
}

/// @brief Search element in the String-Array and return its index.
/// @param base The String Array on which the function will iterate.
/// @param toSearch The searched elements.
/// @return The index of the first elements that equal toSearch if found, -1 else.
int SA_indexOf(stringArr* base, char* toSearch) {
    int index = 0;
    for (int i = 0; i < base->size; i++) {
        if (base->stringArr[i] == NULL) continue;
        if (strcmp(base->stringArr[i], toSearch) == 0) return index;
        index++;
    }

    return -1;
}

/// @brief Transform a string into a String Array based on a space delimitor.
/// @param toParse The string that will be parsed.
/// @return A new String-Array that represents the string from which all the words are separated.
stringArr* SA_parseString(char* toParse) {
    char* toParseTrimed = trim(toParse);
    stringArr* parsed = createStringArray();
    while(*toParseTrimed != '\0'){
        if(*toParseTrimed == ' ') { toParseTrimed++; continue; }

        int wordSize = sizeOfFirstWord(toParseTrimed);
        char* word = Calloc(sizeof(char), (1 + wordSize), "Failed to malloc the word in while parsing.");
        strncat(word, toParseTrimed, wordSize);
        word[wordSize] = '\0';
        
        toParseTrimed += wordSize;
        SA_add(parsed, word);
        free(word);
        if(*toParseTrimed == '\0') return parsed;
    }

    return parsed;
}