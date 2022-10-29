#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "runner.h"
#include "printing.c"
#include "./command/cd.c"
#include "./command/pwd.c"

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