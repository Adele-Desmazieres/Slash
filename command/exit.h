#ifndef EXIT_H
#define EXIT_H
#include "../utils/command.h"

commandResult* exitCommandRunner(command* command, int defaultValue);
int exitArgumentHandler(command* command, int defaultValue);

#endif