#include <string.h>
#include <unistd.h>
#include "./pwd.h"
#include "../utils/command.h"
#include "../utils/printing.h"

#ifndef PWDC
#define PWDC

/* CAS PHYSIQUE (-P) */
commandResult* pwdPhysical(command* command, const char* currPath) {
    char currPhysPath[MAX_ARGS_NUMBER];
    getcwd(currPhysPath, MAX_ARGS_STRLEN);

    return buildCommandResult(TRUE, currPhysPath);
}
/* **************** */


/* CAS LOGIQUE (-L) */
commandResult* pwdLogical(command* command, const char* currPath) {
    char* tmp;
    return buildCommandResult(TRUE, strcpy(tmp, currPath));
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

commandResult* pwdCommandRunner(command* command, const char* currPath) {
    pwdArgumentHandler(command);
    if (command->success == FALSE) return buildCommandResult(FALSE, "");

    //Cas -P : on affiche getcwd()
    switch (command->logicalRef) {
        case TRUE : return pwdLogical(command, currPath);
        case FALSE: return pwdPhysical(command, currPath);
        default   : return buildCommandResult(FALSE, "");
    }
}

#endif