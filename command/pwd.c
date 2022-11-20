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

    return buildCommandResult(TRUE, getcwd(NULL,0));
}
/* **************** */


/* CAS LOGIQUE (-L) */
commandResult* pwdLogical(command* command) {

    return buildCommandResult(TRUE, getenv("PWD"));
}
/* **************** */

void pwdArgumentHandler(command* command) {
    if (command->argNumber == 1 || strcmp( command->args[1], "-L" ) == 0) return;
    if (strcmp( command->args[1], "-P" ) == 0) {
        command->logicalRef = FALSE;
        return;
    }
    
    printError("Invalid argument for the command pwd. Expected argument : -L or -P.\n");
    command->success = FALSE;
}

commandResult* pwdCommandRunner(command* command) {
    pwdArgumentHandler(command);
    if (command->success == FALSE) return buildCommandResult(FALSE, "");

    //Cas -P : on affiche getcwd()
    switch (command->logicalRef) {
        case TRUE : return pwdLogical(command);
        case FALSE: return pwdPhysical(command);
        default   : return buildCommandResult(FALSE, "");
    }
}

#endif