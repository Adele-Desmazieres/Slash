#ifndef EXIT_H
#define EXIT_H
#include "../utils/command.h"

void freeParsedLine(char** parsedLine, int parseLineLength);
commandResult* exitCommandRunner(command* command, char* currPath);

#endif