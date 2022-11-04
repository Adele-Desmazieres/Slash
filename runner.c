#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "runner.h"
#include "printing.h"
#include "command/cd.c"
#include "command/pwd.c"
#include "command/exit.c"


/**
 * @brief Build a new command based on the struct \b command.
 * Which contains : The name of this command, all related arguments and their amount, the success state.
 * @param commandLine The line written by the user to run the command.
 * @param argNumber The number of arguments for the command.
 * @return a pointer command* to the command informations.
 */
command* buildCommand(char** commandLine, int argNumber) {
    command* newCommand = malloc(sizeof(command));
    newCommand->name = commandLine[0];
    newCommand->logicalRef = TRUE;
    newCommand->success = TRUE; //possiblement useless, on verra plus tard...
    newCommand->args = commandLine;
    newCommand->argNumber = argNumber;
    newCommand->targetRef = NULL;

    return newCommand;
}

int commandProcessHandler(command* command, char* currPath) {
    if ( strcmp(command->name, "exit") == 0 ) return exitCommandRunner(command);
    if ( strcmp(command->name, "cd") == 0 ) cdCommandRunner(command, currPath);
    if ( strcmp(command->name, "pwd") == 0 ) pwdCommandRunner(command, currPath);
    //TO-DO : cas des commandes externes
    return 0;
}

int main(int argc, char *argv[]) {
    char* path = "oulalah/cest/trop/long/osskour/aled/je/meurs";
    //char* newPath = reducePathPromptLenght(path);
    //printf("%s", newPath);

    //printPrompt(0, path);
    //printf("\n");

    //A mettre à jour avec chaque appel à cd!
    char *currPath = malloc(sizeof(char)* MAX_ARGS_STRLEN);
    getcwd(currPath, MAX_ARGS_STRLEN);
    if(currPath == NULL) {
        perror("problème repertoire courant"); return -1;
    }

    char* tmp[2] = {"pwd","-P"}; 
    command* comm = buildCommand(tmp, 2);
    commandProcessHandler(comm, currPath);
    readline( printPrompt(0, path) );

    free(currPath);
}