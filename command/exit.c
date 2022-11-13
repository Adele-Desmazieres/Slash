#ifndef EXIT_C
#define EXIT_C
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./exit.h"
#include "../utils/command.h"


commandResult* exitCommandRunner(command* command) {
    if (command->argNumber == 2 && !isdigit(*command->args[1])) {
        return buildFatalCommandResult(TRUE, "Incorrect argument for command 'exit [val]'", 2);
    }
    //TODO utiliser la valeur de retour de la dernière commande à la place de 0
    if (command->argNumber == 1) return buildFatalCommandResult(TRUE, "Successfully exited program.", 0);
    int exitCode = atoi(command->args[1]);

    commandResult* commandResult = buildFatalCommandResult(TRUE, "Successfully exited program.", exitCode);
    return commandResult;
}

#endif