#include "../runner.h"

#ifndef CDC
#define CDC

void cdArgumentHandler(command* command) {
    if (strcmp( command->args[1], "-P" ) == 0) command->logicalRef = FALSE;
    if (strcmp( command->args[1], "-L" ) != 0 || command->argNumber < 3) {
        printError("Invalid argument for the command cd. Expected command format : cd [-L | -P] [ref | -].\n");
        command->success = FALSE;
    }
    command->targetRef = command->args[2];
}

int cdCommandRunner(command* command) {
    cdArgumentHandler(command);
    if (command->success == FALSE) return 1;
    //TO-DO
    return 0;
}

#endif