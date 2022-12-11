#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
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
#include "./utils/jokerSimple.h"

#define PATH_MAX 4096

void readResult(command* command, commandResult* commandResult) {
    if (commandResult->success == FALSE) {
        if (commandResult->resultMessage) printError(commandResult->resultMessage);
        return;
    }
    if (commandResult->resultMessage) {
        printf("%s\n", commandResult->resultMessage);
    }
}

commandResult* commandProcessHandler(command* command, int lastCommandState) {   
    pid_t r;
    int result; 
    if ( strcmp(command->name, "exit") == 0 ) return exitCommandRunner(command, lastCommandState);
    if ( strcmp(command->name, "cd") == 0 ) return cdCommandRunner(command);
    if ( strcmp(command->name, "pwd") == 0 ) return pwdCommandRunner(command);
    
    // expansion des jokers pour les commandes externes
    int *newArgNb = malloc(sizeof(int));
    if (newArgNb == NULL) exit(-1);
    char** expanded = expansionJokers(command->args, command->argNumber, newArgNb);
    //free(command->args);
    command->args = expanded;
    command->argNumber = *newArgNb;
    
    switch(r = fork()) {
        case -1: break;
        case 0: dup2(STDOUT_FILENO, STDERR_FILENO);
                command->args = addFinalNull(command->args, command->argNumber);
                execv(command->name, command->args);
                execvp(command->name, command->args);
                exit(127);
        default: 
                waitpid(r, &result, 0);
                if (WEXITSTATUS(result) == 127) break;
                int tempReturnValue;
                if (WEXITSTATUS(result) == 2 || WEXITSTATUS(result) == 15) 
                    tempReturnValue = 255;
                else tempReturnValue = (WEXITSTATUS(result)) ? 1 : 0;
                return buildCommandResult(!tempReturnValue, NULL);
    }
    //Commande inconnue
    commandResult* finalRes = buildCommandResult(FALSE, "Command unknown.\n");
    return finalRes;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    //Init. de $PATH
    char* currPathTmp = malloc(PATH_MAX * sizeof(char) / 2);
    getcwd(currPathTmp, (PATH_MAX / 2));
    if(currPathTmp == NULL) perror("Erreur getcwd");
    setenv("PWD", currPathTmp, TRUE);
    free(currPathTmp);


    int returnValue = 0; 
    char* line;
    // a mettre à jour avec chaque appel à cd ; PATH_MAX / 4 car PATH_MAX / 2 trop grand


    //readline config
    rl_outstream = stderr;
    using_history();

    //printPrompt(returnValue, getenv("PWD"))
    //Boucle principale
    char* prompt;
    while (( line = readline((prompt = printPrompt(returnValue, getenv("PWD"))))) != NULL) {
        free(prompt);
        char** parsedLine;

        int nbrArgs = countArgs(line);
        if (nbrArgs == 0) continue;

        add_history(line);
        //printf("displayed : %s\n", line);

        // parser la ligne (mots et opérateurs séparés par des espaces)
        parsedLine = Calloc(nbrArgs , sizeof(char *), "Erreur calloc");
        parsedLine = parseLine(line, parsedLine);
        //printParsed(parsedLine, nbrArgs);        
    
        // initier un objet commande puis interprète la commande
        command* commande = buildCommand(parsedLine, nbrArgs);
        
        commandResult* result = commandProcessHandler(commande, returnValue);
        //printParsed(parsedLine, nbrArgs);
        freeParsedLine(parsedLine, nbrArgs);

        if (result->fatal == TRUE) { 
            freeCommand(commande);
            int exitCode = result->exitCode;
            freeCommandResult(result); 
            free(line);
            return exitCode;
        }
        readResult(commande, result);
        freeCommand(commande);
        returnValue = (result->success == TRUE) ? 0 : 1;
        freeCommandResult(result);
        free(line);
    }


    /*étant donné que exit ne fait qu'utiliser la fonction exit(), qui est ici, dans le main, 
    équivalente à un return. Si un utilisateur presse les touche Ctrl+D, il arrivera ici et le
    programme terminera avec la valeur de retour de la dernière commande entrée. Tout comme exit
    sans argument le ferait.
    */
    return returnValue;
}
