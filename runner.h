#ifndef RUNNER_H
#define RUNNER_H

#define TRUE 1
#define FALSE 0
#define PATH_PROMPT_LIMIT 30

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


command* buildCommand(char** command,  int argNumber);
int commandProcessHandler(command* command, char* currPath);

#endif