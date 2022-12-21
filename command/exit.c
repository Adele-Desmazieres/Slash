#ifndef EXIT_C
#define EXIT_C
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./exit.h"
#include "../utils/command.h"

#include <stdio.h>

int exitArgumentHandler(command* command, int defaultValue) {
    if (command->arguments->size == 1) return defaultValue;
    if (command->arguments->size == 2 && !isdigit(*command->arguments->stringArr[1])) {return -1;}
    int exitCode = atoi(command->arguments->stringArr[1]);
    return exitCode;
}


commandResult* exitCommandRunner(command* command, int defaultValue) {
    int exitCode = exitArgumentHandler(command, defaultValue);
    if (exitCode < 0) 
        return buildFatalCommandResult(FALSE, "Incorrect argument for command 'exit [val]'", 1);
    
    return buildFatalCommandResult(TRUE, "Successfully exited program.", exitCode);
}

#endif