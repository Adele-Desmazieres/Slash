#ifndef COMMAND_C
#define COMMAND_C

#include "./command.h"
#include <stdio.h>

/**
 * @brief Build a new command based on the struct \b command.
 * Which contains : The name of this command, all related arguments and their amount, the success state.
 * @param commandLine The line written by the user to run the command.
 * @param argNumber The number of arguments for the command.
 * @return a pointer command* to the command informations.
 */
command* buildCommand(char** commandLine, int argNumber) {
    command* newCommand = Malloc(sizeof(command), "");
    newCommand->name = commandLine[0];
    newCommand->logicalRef = TRUE;
    newCommand->success = TRUE; //possiblement useless, on verra plus tard...
    newCommand->args = commandLine;
    newCommand->argNumber = argNumber;
    newCommand->targetRef = NULL;

    return newCommand;
}

commandResult* buildCommandResult(int success, char* resultString) {
    commandResult* newCommandResult = Malloc(sizeof(commandResult), "");
    newCommandResult->exitCode = -1;
    newCommandResult->fatal = FALSE;
    newCommandResult->success = success;
    newCommandResult->resultMessage = resultString;

    return newCommandResult;
}

commandResult* buildFatalCommandResult(int success, char* errorMessage, int exitCode) {
    commandResult* newCommandResult = Malloc(sizeof(commandResult), "");
    newCommandResult->exitCode = exitCode;
    newCommandResult->fatal = TRUE;
    newCommandResult->success = success;
    newCommandResult->resultMessage = errorMessage;

    return newCommandResult;
}

#endif