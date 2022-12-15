#ifndef COMMAND_H
#define COMMAND_H 1

#define TRUE 1
#define FALSE 0

#define SUCCESS 101
#define ERROR 100

#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096

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

void freeCommandResult(commandResult* cr);
void freeCommand (command* c);
command* buildCommand(char** command,  int argNumber);
void alterCommandArgs(command *c, char** newArgs, int newArgNumber);
commandResult* buildCommandResult(int success, char* resultString);
commandResult* buildFatalCommandResult(int success, char* errorMessage, int exitCode);

#endif