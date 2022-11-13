#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "./cd.h"
#include "../utils/command.h"
#include "../utils/printing.h"

#include <stdio.h> // temporaire pour debugger

#ifndef CDC
#define CDC

/* CAS PHYSIQUE (-P) */
commandResult* cdPhysical(command* command, char* currPath) {
    char* currPhysPath = malloc(sizeof(char) * MAX_ARGS_NUMBER);
    printf("0\n");
    // si le processus actuel réussi à changer de current working directory
    // et qu'on arrive à récuperer le nouveau directory grace à cwd
    if (chdir(command->args[2]) == 0 && getcwd(currPhysPath, MAX_ARGS_STRLEN)) { 
        
        printf("A %s\n", currPhysPath);

        // modifie le current path
        if (realloc(currPath, sizeof(char) * strlen(currPhysPath)) == NULL) {
            printf("D\n");
            free(currPhysPath);
            return buildCommandResult(FALSE, ""); // renvoie un échec
        };
        strcpy(currPath, currPhysPath);

        // TODO : tester
        // TODO : maintenir à jour env var $OLDPWD, pour cd -
        
        return buildCommandResult(TRUE, currPhysPath); // renvoie une réussite

    } else {
        printf("B\n");
        free(currPhysPath);
        return buildCommandResult(FALSE, ""); // renvoie un échec
    }
}
/* **************** */

char** split(const char* path) { // renvoie la liste des pointeurs des mots
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
    printf("%s\n", command->args[1]);
    if (strcmp( command->args[1], "-P" ) == 0) command->logicalRef = FALSE;
    else if (strcmp( command->args[1], "-L" ) != 0 || command->argNumber < 3) {
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