#ifndef RUNNER_H
#define RUNNER_H

#define PATH_PROMPT_LIMIT 30

#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096

void* Malloc(size_t size, char* errorMessage);
void readResult(command* command, commandResult* commandResult);
commandResult* commandProcessHandler(command* command, char* currPath);


#endif