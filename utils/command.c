#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "StringArray.h"
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
command* buildCommand(char* commandLine) {
    command* newCommand = Malloc(sizeof(command), "");
    newCommand->logicalRef = TRUE;
    newCommand->success = TRUE; //possiblement useless, on verra plus tard...
    newCommand->arguments = SA_parseString(commandLine);
    newCommand->name = newCommand->arguments->stringArr[0];
    newCommand->targetRef = NULL;
    newCommand->redirect.error = STDERR_FILENO;
    newCommand->redirect.output = STDOUT_FILENO;
    newCommand->redirect.input = STDIN_FILENO;

    return newCommand;
}

command* buildCommandParsed(stringArr* args) {
    command* newCommand = Malloc(sizeof(command), "");
    newCommand->logicalRef = TRUE;
    newCommand->success = TRUE; //possiblement useless, on verra plus tard...
    newCommand->arguments = args;
    newCommand->name = (args->size == 0) ? NULL : args->stringArr[0];
    newCommand->targetRef = NULL;
    newCommand->redirect.error = STDERR_FILENO;
    newCommand->redirect.output = STDOUT_FILENO;
    newCommand->redirect.input = STDIN_FILENO;

    return newCommand;
}

void alterCommandArgs(command *c, stringArr* newArgs) {
    SA_free(c->arguments);
    c->arguments = newArgs;
    c->name = newArgs->stringArr[0];
}

int redirectErr(command *c, int newErrorOutput) {
    if (newErrorOutput < 0) { perror("FILE"); return FALSE; }
    c->redirect.error = newErrorOutput;
    return TRUE;
}

int redirectOutput(command *c, int newOutput) {
    if (newOutput < 0) { perror("FILE"); return FALSE; }
    c->redirect.output = newOutput;
    return TRUE;
}

int redirectInput(command *c, int newInput) {
    if (newInput < 0) { perror("FILE"); return FALSE; }
    c->redirect.input = newInput;
    return TRUE;
}

void freeCommand (command* c){
    free(c->targetRef);
    SA_free(c->arguments);
    free(c);
    return;
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
    newCommandResult->resultMessage = NULL;
    if(resultString) {
        newCommandResult->resultMessage = Malloc(((strlen(resultString) + 1) * sizeof(char)), "");
        strcpy(newCommandResult->resultMessage, resultString);
    }

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

    newCommandResult->resultMessage = Malloc(((strlen(errorMessage) + 1) * sizeof(char)), "");
    strcpy(newCommandResult->resultMessage, errorMessage);

    return newCommandResult;
}

void freeCommandResult (commandResult* c){
    free(c->resultMessage);
    free(c);
    return;
}

#endif