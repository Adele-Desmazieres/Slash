#ifndef EXIT_C
#define EXIT_C
#include <stdlib.h>
#include <string.h>
#include "./exit.h"
#include "../utils/command.h"


commandResult* exitCommandRunner(command* command) {
    if (command->argNumber == 1) {
        char* errMsg = malloc(41 * sizeof(char));
        strcpy(errMsg, "Argument for the command exit is missing");
        return buildCommandResult(FALSE, errMsg);
    }
    int exitCode = atoi(command->args[1]);

    commandResult* commandResult = buildFatalCommandResult(TRUE, "Successfully exited program.", exitCode);
    return commandResult;
}

#endif