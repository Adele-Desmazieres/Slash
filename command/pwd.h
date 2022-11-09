#ifndef PWD_H
#define PWD_H
#include "../utils/command.h"

void pwdArgumentHandler(command* command);
commandResult* pwdCommandRunner(command* command, const char* currPath);

#endif