#ifndef EXIT_C
#define EXIT_C
#include <stdlib.h>
#include "./exit.h"
#include "../utils/command.h"


void freeParsedLine(char** parsedLine, int parseLineLength) {
    for (int i = 0; i < 0; i++) {
        free(parsedLine[i]);
    }
    free(parsedLine);
}

commandResult* exitCommandRunner(command* command) {
    if (command->argNumber == 1) 
        return buildCommandResult(FALSE, "Argument for the command exit is missing");
    int exitCode = atoi(command->args[1]);

    commandResult* commandResult = buildFatalCommandResult(TRUE, "Successfully exited program.", exitCode);
    return commandResult;
}

#endif