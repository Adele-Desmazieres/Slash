#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "runner.h"

#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096

/**
 * @brief Build a new command based on the struct \b command.
 * Which contains : The name of this command, all related arguments and their amount, the success state.
 * @param commandLine The line written by the user to run the command.
 * @param argNumber The number of arguments for the command.
 * @return a pointer command* to the command informations.
 */
command* buildCommand(char** commandLine, int argNumber) {
    command* newCommand = malloc(sizeof(command));
    newCommand->name = commandLine[0];
    newCommand->logicalRef = TRUE;
    newCommand->success = TRUE; //possiblement useless, on verra plus tard...
    newCommand->args = commandLine;
    newCommand->argNumber = argNumber;
    newCommand->targetRef = NULL;

    return newCommand;
}

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
 * @brief Print a prompt line as expected following this format : "[(lastCommandResult)](currentPath)$ "
 * 
 * @param lastCommandResult the result of the last command runned in the smash.
 * @param path the path of the current directory
 */
void printPrompt(int lastCommandResult, char* path) {
    char* tmp = malloc(sizeof(char)*4);
    sprintf(tmp, "[%d]", lastCommandResult);
    if (lastCommandResult == 1) {
        printError( tmp );
    }else {
        printSuccess( tmp );
    }
    char* pathToPrint = path;
    if (strlen(path) > 30) {
        pathToPrint = reducePathPromptLenght(path);
    }
    printWithColor(pathToPrint, "\033[34m");
    printWithColor("$ ", "\033[36m");
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

int exitCommandRunner(command* command) {
    if (strcmp(command->args[1], "0") != 0 && strcmp(command->args[1], "1")) {
        command->success = FALSE;
        printError("Invalid argument for the command exit. Expected argument : 0 or 1.\n");
        return 0;
    }
    int exitCode = atoi(command->args[1]);
    exit(exitCode);
}

void pwdArgumentHandler(command* command) {
    if (strcmp( command->args[1], "-P" ) == 0) {
        command->logicalRef = FALSE;
        return;
    }
    if (strcmp( command->args[1], "-L" ) == 0) return;
    
    printError("Invalid argument for the command pwd. Expected argument : -L or -P.\n");
    command->success = FALSE;
}

void cdArgumentHandler(command* command) {
    if (strcmp( command->args[1], "-P" ) == 0) command->logicalRef = FALSE;
    if (strcmp( command->args[1], "-L" ) != 0 || command->argNumber < 3) {
        printError("Invalid argument for the command cd. Expected command format : cd [-L | -P] [ref | -].\n");
        command->success = FALSE;
    }
    command->targetRef = command->args[2];
}

int cdCommandRunner(command* command) {
    cdArgumentHandler(command);
    if (command->success == FALSE) return 1;
    //TO-DO
    return 0;
}

int pwdCommandRunner(command* command) {
    pwdArgumentHandler(command);
    if (command->success == FALSE) return 1;

    char currPath[MAX_ARGS_STRLEN];
    getcwd(currPath, MAX_ARGS_STRLEN);
    printf("%s\n", "-------------------");
    printf("%s\n", currPath);
    printf("%s\n", "-------------------");
    command->success = TRUE;
    return 0;
}

int commandProcessHandler(command* command) {
    if ( strcmp(command->name, "exit") == 0 ) return exitCommandRunner(command);
    if ( strcmp(command->name, "cd") == 0 ) cdCommandRunner(command);
    if ( strcmp(command->name, "pwd") == 0 ) pwdCommandRunner(command);
    //TO-DO : cas des commandes externes
    return 0;
}

int main(int argc, char *argv[]) {
    char* path = "oulalah/cest/trop/long/osskour/aled/je/meurs";
    //char* newPath = reducePathPromptLenght(path);
    //printf("%s", newPath);

    //printPrompt(0, path);
    //printf("\n");
    char* tmp[2] = {"pwd","-P"}; 
    command* comm = buildCommand(tmp, 2);
    commandProcessHandler(comm);
}