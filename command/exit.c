#include "runner.h"

#ifndef EXIT_C
#define EXIT_C

int exitCommandRunner(command* command) {
    if (strcmp(command->args[1], "0") != 0 && strcmp(command->args[1], "1")) {
        command->success = FALSE;
        printError("Invalid argument for the command exit. Expected argument : 0 or 1.\n");
        return 0;
    }
    int exitCode = atoi(command->args[1]);
    exit(exitCode);
}

#endif