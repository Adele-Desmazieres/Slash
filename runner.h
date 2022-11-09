#include "./utils/command.h"
#ifndef RUNNER_H
#define RUNNER_H
#define PATH_PROMPT_LIMIT 30

void readResult(command* command, commandResult* commandResult);
commandResult* commandProcessHandler(command* command, char* currPath);


#endif