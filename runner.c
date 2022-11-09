#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "runner.h"
#include "./utils/memory.h"
#include "./utils/command.h"
#include "./utils/lineParser.h"
#include "./utils/printing.h"
#include "./command/cd.h"
#include "./command/pwd.h"
#include "./command/exit.h"

#define PATH_MAX 4096

void readResult(command* command, commandResult* commandResult) {
    if (commandResult->success == FALSE) {
        printError(commandResult->resultMessage);
        return;
    }

    printf("%s\n", commandResult->resultMessage);
}

commandResult* commandProcessHandler(command* command, char* currPath) {    
    if ( strcmp(command->name, "exit") == 0 ) return exitCommandRunner(command, currPath);
    if ( strcmp(command->name, "cd") == 0 ) return cdCommandRunner(command, currPath);
    if ( strcmp(command->name, "pwd") == 0 ) return pwdCommandRunner(command, currPath);
    //TO-DO : cas des commandes externes

    //Commande inconnue
    return buildCommandResult(FALSE, "Command unknown.");
}

int main(int argc, char *argv[]) {
    int returnValue = 0; 
    char* line      = Malloc(sizeof(char) * MAX_ARGS_STRLEN, "");
    char* currPath  = Malloc((PATH_MAX / 4) * sizeof(char), "problème repertoire courant");
    // a mettre à jour avec chaque appel à cd ; PATH_MAX / 4 car PATH_MAX / 2 trop grand

    getcwd(currPath, (PATH_MAX / 4));

    //readline config
    rl_outstream = stderr;
    using_history();

    while ((line = readline(printPrompt(returnValue,currPath))) != NULL) {
        char** parsedLine;
        add_history(line); 

        printf("displayed : %s\n", line);

        // parser la ligne (mots et opérateurs séparés par des espaces)
        int nbrArgs = countArgs(line);
        if (nbrArgs == 0) continue;

        parsedLine = Calloc(nbrArgs , sizeof(char *), "Erreur calloc");
        parsedLine = parseLine(line, parsedLine);
        //printParsed(parsedLine, nbrArgs);        
    
        // initier un objet commande puis interprète la commande
        command* commande = buildCommand(parsedLine, nbrArgs);
        commandResult* result = commandProcessHandler(commande, currPath);
        freeParsedLine(parsedLine, nbrArgs);

        if (result->fatal == TRUE) { 
            free(commande); 
            exit(result->exitCode); 
        }

        readResult(commande, result);
        free(commande);
        returnValue = (result->success == TRUE) ? 0 : 1;
    }
    
    return 1;
}