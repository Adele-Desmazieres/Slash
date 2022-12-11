#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "lineParser.h"


int countArgs(const char* c) {
    
    while(!isalnum(*c)){
        if(*c == '\0') return 0;
        c++;
    } 
    int count = 1;
    while ( *c != '\0'){
        if (*c == ' '){
            count++;
            while (*c == ' '){
                if (*c == '\0') return count;
                c++;
            } 
        }
        c++;
    }
    return count;
}

int sizeOfToken (const char* c){
    int size = 0;
    while(*c != '\0' && *c != ' '){
        size++; c++;
    }
    return size;
}

char** parseLine(const char* line, char** parsedLine) {
    while(!isalnum(*line) && *line != '/' && *line == '.'){
        if (*line == '\0') return parsedLine;   
        line++;
    }
    
    int iterator = 0;
    while(*line != '\0'){
        if(*line != ' '){
            int tokenSize = sizeOfToken(line);
            parsedLine[iterator] = calloc (sizeof(char) , (1 + tokenSize));
            strncat(parsedLine[iterator], line, tokenSize);
            parsedLine[iterator++][tokenSize] = '\0';
            line += tokenSize;
            if(*line == '\0') return parsedLine; 
        }
        line++;
    }
    
    return parsedLine;
}

char** addFinalNull(char** parsedLine, int size) {
    int newSize = size+1;
    char** test = realloc(parsedLine, newSize*sizeof(char*));
    test[size] = NULL;
    return test;
}

void printParsed(char ** parsed, int len){
    for(int i = 0; i < len; i++) {
        if(parsed[i] == NULL) {
            printf(" | argument %d : NULL\n", i+1); 
            continue;
        }
        printf(" | argument %d : %s-\n", i+1, parsed[i]);
    }
}

void freeParsedLine(char** parsedLine, int parseLineLength) {
    for (int i = 0; i < parseLineLength; i++) {
        if(parsedLine[i] != NULL) {
            free(parsedLine[i]);
        }else {
            printf(" | argument %d : NULL \n", i+1);
        }
    }
    free(parsedLine);
}
