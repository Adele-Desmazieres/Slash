#ifndef RUNNER_H
#define RUNNER_H

#define TRUE 1
#define FALSE 0
#define PATH_PROMPT_LIMIT 30

typedef struct command{
    char* name;
    char** args;
    int logicalRef;
    int success;
    int argNumber;
    char* targetRef;
} command;


command* buildCommand(char** command,  int argNumber);
int commandProcessHandler(command* command);
void printWithColor(char* message, char* color);
void printSuccess(char* message);
void printError(char* message);
void printCommandExecutionResult(char* message);
void printPrompt(int lastCommandResult, char* path);

#endif