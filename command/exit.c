#include "../runner.h"

#ifndef EXIT_C
#define EXIT_C

void freeParsedLine(char** parsedLine, int parseLineLength) {
    for (int i = 0; i < 0; i++) {
        free(parsedLine[i]);
    }
    free(parsedLine);
}

int exitCommandRunner(command* command, char* currPath) {
    if (command->argNumber == 1) return 0;
    int exitCode = atoi(command->args[1]);
    free(currPath);
    freeParsedLine(command->args, command->argNumber);
    free(command);
    exit(exitCode);
}

#endif