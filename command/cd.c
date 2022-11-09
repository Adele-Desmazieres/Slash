#ifndef CDC
#define CDC
#include "../utils/command.c"

/* CAS PHYSIQUE (-P) */
commandResult* cdPhysical(command* command, const char* currPath) {
    char currPhysPath[MAX_ARGS_NUMBER];

    chdir(command->args[2]); // le processus actuel change de current working directory

    if (getcwd(currPhysPath, MAX_ARGS_STRLEN)) { // maintient à jour du chemin vers ce directory
        return buildCommandResult(TRUE, currPhysPath);
    }

    // TODO : maintenir à jour env var $OLDPWD, pour cd -
    // TODO : tester
        
    return buildCommandResult(FALSE, "");
}
/* **************** */

char** split(const char* path) { // renvoie la liste des pointeurs
    // TODO
    return NULL;
}

/* CAS LOGIQUE (-L) */
commandResult* cdLogical(command* command, const char* currPath) {
    // TODO
    char* targetPath;
    char* absoluteTargetPath;
    strcpy(targetPath, command->args[2]);
    strcpy(absoluteTargetPath, currPath);

    while(split(currPath)) { // parcourir les tokens du string chemin, séparés par des /
        // TODO
    }


    return buildCommandResult(TRUE, absoluteTargetPath);
}
/* **************** */


void cdArgumentHandler(command* command) {
    if (strcmp( command->args[1], "-P" ) == 0) command->logicalRef = FALSE;
    if (strcmp( command->args[1], "-L" ) != 0 || command->argNumber < 3) {
        printError("Invalid argument for the command cd. Expected command format : cd [-L | -P] [ref | -].\n");
        command->success = FALSE;
    }
    /* TODO 
    Cas particuliers :
    cd - : retourne au dernier répertoire, contenu dans une var d'environnement $OLDPWD
    cd : retourne au répertoire ~ contenu dans la var d'environnement $HOME
    cd path = cd -L path

    Gerer var environnement :
    setenv
    getenv
    */
    command->targetRef = command->args[2];
}

//Ne pas oublier de mettre à jour currPath!
commandResult* cdCommandRunner(command* command, char* currPath) {
    cdArgumentHandler(command);
    if (command->success == FALSE) return buildCommandResult(FALSE, "");

    commandResult* commandResult = buildCommandResult(TRUE, "");

    switch (command->logicalRef) {
        case TRUE : commandResult = cdLogical(command, currPath);
        case FALSE: commandResult = cdPhysical(command, currPath);
        default   : commandResult = buildCommandResult(FALSE, "");
    }

    return commandResult;
}



#endif