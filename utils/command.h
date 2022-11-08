#ifndef COMMAND_H
#define COMMAND_H

#define TRUE 1
#define FALSE 0

#define SUCCESS 101
#define ERROR 100

typedef struct command{
    char* name;
    char** args;
    int logicalRef;
    int success;
    int argNumber;
    //targetRef servira pour la commande CD pour stocker le chemin de destination
    char* targetRef;
} command;

typedef struct commandResult{
    int fatal;
    int success;
    int exitCode;
    char* resultMessage;
} commandResult;

command* buildCommand(char** command,  int argNumber);
commandResult* buildCommandResult(int success, char* resultString);
commandResult* buildFatalCommandResult(int success, char* errorMessage, int exitCode);

#endif