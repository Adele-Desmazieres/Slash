#include "../utils/command.c"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

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
    return buildCommandResult(TRUE, currPath);
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