#include <stdio.h>
#include "StringArray.h"

#ifndef COMMAND_H
#define COMMAND_H 1

#define TRUE 1
#define FALSE 0

#define SUCCESS 0
#define ERROR 1

#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096

typedef struct redirection {
    int input;
    int error;
    int output;
} redirection;

typedef struct command{
    char* name;
    stringArr* arguments;
    int logicalRef;
    int success;
    //targetRef servira pour la commande CD pour stocker le chemin de destination
    char* targetRef;
    redirection redirect;
} command;

typedef struct commandResult{
    int fatal;
    int success;
    int exitCode;
    int statusExited;
    char* resultMessage;
} commandResult;

void freeCommandResult(commandResult* cr);
void freeCommand (command* c);
command* buildCommand(char* commandLine);
command* buildCommandParsed(stringArr* args);
void alterCommandArgs(command *c, stringArr* newArgs);
commandResult* buildCommandResult(int success, char* resultString);
commandResult* buildFatalCommandResult(int success, char* errorMessage, int exitCode);
int redirectErr(command *c, int newErrorOutput);
int redirectOutput(command *c, int newOutput);
int redirectInput(command *c, int newInput);

#endif