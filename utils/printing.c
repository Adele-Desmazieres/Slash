#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "printing.h"

/**
 * @brief Reduce the lenght of a string to 30 or less characters.
 * 
 * @param currPath 
 * @return if the currPath length is higher than 30, return a new char* following
 * this format : "..." followed with the 27 lasts characters of currPath, else return currPath; 
 */
char* reducePathPromptLenght(char* currPath) {
    if (strlen(currPath) < 30) return currPath;
    char* reducedPath = calloc(sizeof(char) * 31, sizeof(char));
    strcat(reducedPath, "...");
    char* keptPart = (char*) currPath + strlen(currPath) - 27;
    strcat(reducedPath, keptPart);
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
    char* tmp = malloc(sizeof(char)*37);

    char* pathToPrint = path;
    if (strlen(path) > 30) {
        pathToPrint = reducePathPromptLenght(path);
    }

    if (lastCommandResult == 1) {
        sprintf(tmp, "\033[0m[\033[91m%d\033[0m]\033[34m%s\033[36m$ \033[0m", lastCommandResult, pathToPrint);
    }else {
        sprintf(tmp, "\033[0m[\033[32m%d\033[0m]\033[34m%s\033[36m$ \033[0m", lastCommandResult, pathToPrint);
    }

    return tmp;
}