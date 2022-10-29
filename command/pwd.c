#include "runner.h"

#ifndef PWDC
#define PWDC

void pwdArgumentHandler(command* command) {
    if (strcmp( command->args[1], "-P" ) == 0) {
        command->logicalRef = FALSE;
        return;
    }
    if (strcmp( command->args[1], "-L" ) == 0) return;
    
    printError("Invalid argument for the command pwd. Expected argument : -L or -P.\n");
    command->success = FALSE;
}

int pwdCommandRunner(command* command) {
    pwdArgumentHandler(command);
    if (command->success == FALSE) return 1;

    char currPath[MAX_ARGS_STRLEN];
    getcwd(currPath, MAX_ARGS_STRLEN);
    printf("%s\n", "-------------------");
    printf("%s\n", currPath);
    printf("%s\n", "-------------------");
    command->success = TRUE;
    return 0;
}

#endif