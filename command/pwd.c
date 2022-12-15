#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "./pwd.h"
#include "../utils/command.h"
#include "../utils/printing.h"

#ifndef PWDC
#define PWDC

/* CAS PHYSIQUE (-P) */
commandResult* pwdPhysical(command* command) {
    char* cwd = getcwd(NULL,0);
    commandResult* result = buildCommandResult(SUCCESS, cwd);
    free(cwd);
    return result;
}
/* **************** */


/* CAS LOGIQUE (-L) */
commandResult* pwdLogical(command* command) {

    return buildCommandResult(SUCCESS, getenv("PWD"));
}
/* **************** */

void pwdArgumentHandler(command* command) {
    if (command->argNumber == 1 || strcmp( command->args[1], "-L" ) == 0) return;
    if (strcmp( command->args[1], "-P" ) == 0) {
        command->logicalRef = FALSE;
        return;
    }
    
    printError("Invalid argument for the command pwd. Expected argument : -L or -P.\n");
    command->success = ERROR;
}

commandResult* pwdCommandRunner(command* command) {
    pwdArgumentHandler(command);
    if (command->success == FALSE) return buildCommandResult(ERROR, "");

    //Cas -P : on affiche getcwd()
    switch (command->logicalRef) {
        case TRUE : return pwdLogical(command);
        case FALSE: return pwdPhysical(command);
        default   : return buildCommandResult(ERROR, "");
    }
}

#endif