#include "../runner.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

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

int pwdCommandRunner(command* command, const char* currPath) {
    pwdArgumentHandler(command);
    if (command->success == FALSE) return 1;

    //Cas -P : on affiche getcwd()
    if (command->logicalRef == FALSE){
        char currPhysPath[MAX_ARGS_STRLEN];
        getcwd(currPhysPath, MAX_ARGS_STRLEN);
        printf("%s\n", "-------------------");
        printf("%s\n", currPhysPath);
        printf("%s\n", "-------------------");
        command->success = TRUE;
        return 0;
    //Cas -L : on affiche le chemin stocké dans le main, pwd est donc dépendant de cd...
    } else {
        printf("%s\n", "-------------------");
        printf("%s\n", currPath);
        printf("%s\n", "-------------------");
        command->success = TRUE;
        return 0;  
    }
    command->success = FALSE;
    return 1;
    
}

#endif