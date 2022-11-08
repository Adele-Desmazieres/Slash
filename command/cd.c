#ifndef CDC
#define CDC
#include "../utils/command.c"

void cdArgumentHandler(command* command) {
    if (strcmp( command->args[1], "-P" ) == 0) command->logicalRef = FALSE;
    if (strcmp( command->args[1], "-L" ) != 0 || command->argNumber < 3) {
        printError("Invalid argument for the command cd. Expected command format : cd [-L | -P] [ref | -].\n");
        command->success = FALSE;
    }
    command->targetRef = command->args[2];
}

//Ne pas oublier de mettre à jour currPath!
commandResult* cdCommandRunner(command* command, char* currPath) {
    cdArgumentHandler(command);
    if (command->success == FALSE) return buildCommandResult(FALSE, "");

    commandResult* commandResult = buildCommandResult(TRUE, "");
    return commandResult;
}

#endif