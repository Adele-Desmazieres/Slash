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
    resetPrintColor();
}

commandResult* commandProcessHandler(command* command) {    
    if ( strcmp(command->name, "exit") == 0 ) return exitCommandRunner(command);
    if ( strcmp(command->name, "cd") == 0 ) return cdCommandRunner(command);
    if ( strcmp(command->name, "pwd") == 0 ) return pwdCommandRunner(command);
    //TO-DO : cas des commandes externes

    //Commande inconnue
    char* errMsg = malloc(20 * sizeof(char));
    strcpy(errMsg, "Command unknown.\n");
    return buildCommandResult(FALSE, errMsg);
}

int main(int argc, char *argv[]) {

    //Init. de $PATH
    char* currPathTmp = malloc(PATH_MAX * sizeof(char) / 2);
    getcwd(currPathTmp, (PATH_MAX / 2));
    if(currPathTmp == NULL) perror("Erreur getcwd");
    setenv("PWD", currPathTmp, TRUE);
    free(currPathTmp);


    int returnValue = 0; 
    char* line      = Malloc(sizeof(char) * MAX_ARGS_STRLEN, "");
    // a mettre à jour avec chaque appel à cd ; PATH_MAX / 4 car PATH_MAX / 2 trop grand


    //readline config
    rl_outstream = stderr;
    using_history();

    //printPrompt(returnValue, getenv("PWD"))
    //Boucle principale
    while ((line = readline(printPrompt(returnValue, getenv("PWD")))) != NULL) {
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
        

        commandResult* result = commandProcessHandler(commande);

        freeParsedLine(parsedLine, nbrArgs);


        if (result->fatal == TRUE) { 
            freeCommand(commande); 
            exit(result->exitCode); 
        }

        readResult(commande, result);
        freeCommand(commande);
        returnValue = (result->success == TRUE) ? 0 : 1;
        free(result);

    }
    
    return 1;
}