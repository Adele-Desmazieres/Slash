#include "../runner.h"

#ifndef EXIT_C
#define EXIT_C

int exitCommandRunner(command* command) {
    if (command->argNumber == 1) return 0;
    int exitCode = atoi(command->args[1]);
    return exitCode;
}

#endif