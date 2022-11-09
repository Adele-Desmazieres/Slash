#include <stdlib.h>
#include <stdio.h>
#include "command.h"
#include "./memory.h"

#ifndef COMMAND_C
#define COMMAND_C

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

/**
 * @brief Build a new command result based on the struct \b commandResult.
 * Which contains : 
 * - \b success       : int that represents the success or not of the command.
 * - \b fatal         : int that determinate if the execution is fatal for the slash.
 *                      or not (always FALSE here)
 * - \b exitCode      : int that will be used to exit the program (unused here.)
 * - \b resultMessage : string that will be displayed as a result of the command 
 * @param success the boolean that represents the success or not of the command.
 * @param resultString The string that will be displayed as a result of the command
 * @return a pointer commandResult*.
 */
commandResult* buildCommandResult(int success, char* resultString) {
    commandResult* newCommandResult = Malloc(sizeof(commandResult), "");
    newCommandResult->exitCode = -1;
    newCommandResult->fatal = FALSE;
    newCommandResult->success = success;
    newCommandResult->resultMessage = resultString;

    return newCommandResult;
}

/**
 * @brief Build a new command result based on the struct \b commandResult.
 * Which contains : 
 * - \b success       : int that represents the success or not of the command. \newline
 * - \b fatal         : int that determinate if the execution is fatal for the slash 
 *                      or not (always FALSE here)
 * - \b exitCode      : int that will be used to exit the program (unused here.)
 * - \b resultMessage : string that will be displayed as a result of the command 
 * @param success the boolean that represents the success or not of the command.
 * @param errorMessage The string that will be displayed as error warning.
 * @param exitCode the code that will be used to end the program.
 * @return a pointer commandResult*.
 */
commandResult* buildFatalCommandResult(int success, char* errorMessage, int exitCode) {
    commandResult* newCommandResult = Malloc(sizeof(commandResult), "");
    newCommandResult->exitCode = exitCode;
    newCommandResult->fatal = TRUE;
    newCommandResult->success = success;
    newCommandResult->resultMessage = errorMessage;

    return newCommandResult;
}

#endif