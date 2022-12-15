#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "printing.h"

/**
 * @brief Reduce the lenght of a string to 30 or less characters.
 * 
 * @return if the currPath length is higher than 30, return a new char* following
 * this format : "..." followed with the 27 lasts characters of currPath, else return currPath; 
 */
char* reducePathPromptLenght(int maxLength) {
    char* currPath = malloc (sizeof(char) * (strlen(getenv("PWD")) + 1));
    strcpy (currPath, getenv("PWD"));
    if (strlen(currPath) < maxLength) return currPath;
    char* reducedPath = calloc(sizeof(char) * 31, sizeof(char));
    strcat(reducedPath, "...");
    //-3 à cause des point de suspension devant
    char* keptPart = (char*) currPath + strlen(currPath) - (maxLength - 3);
    strcat(reducedPath, keptPart);
    free(currPath);
    return reducedPath;
}

/**
 * @brief
 * Reset the text color on the prompt.
 * Shouldn't be useful according to how \b printWithColor works.
 * 
 */
void resetPrintColor() { printf("\033[0m"); }

/**
 * @brief
 * Print the content of the \b message argument with a specific font color ( according to \b color )
 * @param message The message that will be displayed
 * @param color The font color of this message.
 */
void printWithColor(char* message, char* color) {
    printf("%s", color);
    printf("%s", message);
    resetPrintColor();
}


/**
 * @brief
 * Print the content of the \b message argument with a green font color
 * @param message The message that will be displayed
 */
void printSuccess(char* message) {
    printWithColor(message, "\033[32m");
}

/**
 * @brief
 * Print the content of the \b message argument with a red font color
 * @param message The message that will be displayed
 */
void printError(char* message) {
    printWithColor(message, "\033[91m");
}

/**
 * @brief Print a prompt line as expected following this format : "[(lastCommandResult)](currentPath)$ "
 * 
 * @param lastCommandResult the result of the last command runned in the smash.
 * @param path the path of the current directory
 */
char* printPrompt(int lastCommandResult, char* path) {
    char* tmp = malloc(sizeof(char)*48);
    int reduced = 0;
    char* pathToPrint = path;
    int resultCodeLen = 0;
    if (lastCommandResult >=10) resultCodeLen = floor(log10(abs(lastCommandResult)));

    int maxPathLenght = MAX_PATH_PROMPT_LENGTH - resultCodeLen;
    if (strlen(path) > maxPathLenght) {
        reduced = 1;
        pathToPrint = reducePathPromptLenght(maxPathLenght);
    }

    if (lastCommandResult == 0) {
        snprintf(tmp, 46+resultCodeLen, "\033[32m[%d]\033[34m%s\033[00m$ ", lastCommandResult, pathToPrint);
    }else if (lastCommandResult == 255) {
        snprintf(tmp, 48, "\033[33m[SIG]\033[34m%s\033[00m$ ", pathToPrint);
    }else {
        snprintf(tmp, 46+resultCodeLen, "\033[91m[%d]\033[34m%s\033[00m$ ", lastCommandResult, pathToPrint);
    }

    if(reduced) free(pathToPrint);

    return tmp;
}