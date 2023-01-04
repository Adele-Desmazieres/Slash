#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "runner.h"
#include "./process.h"
#include "./utils/memory.h"
#include "./utils/printing.h"

#define PATH_MAX 4096

void freeSA_array(stringArr** commands, int amount) {
    for (int i = 0; i < amount; i++) {
        if (commands[i] != NULL) SA_free(commands[i]);
    }
    free(commands);
}

void freeCommandArrays(command** commands, int amount) {
    for (int i = 0; i < amount; i++) {
        if (commands[i] != NULL) freeCommand(commands[i]);
    }
    free(commands);
}

int checkIfEmptyCommand(stringArr** commands, int amount) {
    for (int i = 0; i < amount; i++) {
        if (commands[i]->size == 0) return TRUE;
    }
    return FALSE;
}

void freePipeArray(int** tab, int size) {
    for (int i = 0; i < size; i++) { free(tab[i]); }
    free(tab);
}

int** createPipes(int n) {
    int** tubes = malloc(sizeof(int*)*n);

    for (int i = 0; i < n; i++) {
        tubes[i] = malloc(sizeof(int)*2);
        pipe(tubes[i]);
    }
    
    return tubes;
}

int main(int argc, char *argv[]) {
    char* line;
    char* prompt;
    int returnValue = 0;
    int lastWasSignalTerminated = FALSE;
    initialSIGBehavior last;
    struct sigaction ignore = {0};
    ignore.sa_handler= SIG_IGN;

    sigaction(SIGINT, &ignore, &last.sig_int);
    sigaction(SIGTERM, &ignore, &last.sig_term);
    // signal(SIGINT, SIG_IGN);
    // signal(SIGTERM, SIG_IGN);
    //Init. de $PATH
    char* currPathTmp = Malloc(PATH_MAX * sizeof(char) / 2, "Erreur malloc getcwd");
    getcwd(currPathTmp, (PATH_MAX / 2));
    setenv("PWD", currPathTmp, TRUE);
    free(currPathTmp);

    //readline config
    rl_outstream = stderr;
    using_history();

    //Boucle principale
    while (( line = readline((prompt = printPrompt(returnValue, lastWasSignalTerminated, getenv("PWD"))))) != NULL) {
        free(prompt);
        add_history(line);
        char missingQuote = checkIfQuotesAreClosed(line);
        if (missingQuote != ((char) 0)) line = completeLine(line, missingQuote);
        //printf("displayed : %s\n", line);        
        int* commandAmount = Malloc(sizeof(int), "Erreur malloc, command Amount l.93 !");
        
        stringArr* parsed = SA_parseStringWithQuotes(line);
        free(line);
        if (parsed->size == 0) {
            free(commandAmount);
            SA_free(parsed);
            continue;
        }
        stringArr** commandsArgs = SA_split(parsed, "|", commandAmount);
        SA_free(parsed);
        if (checkIfEmptyCommand(commandsArgs, *commandAmount)) {
            freeSA_array(commandsArgs, *commandAmount);
            free(commandAmount);
            returnValue = 1; continue;
        }
        
        command** tableauCommand = malloc(sizeof(command) * (*commandAmount));
        if (tableauCommand == NULL) perror("error malloc");
        
        // transformer le tableau de string en tableau de commandes et malloc les pipes
        int **pipes = createPipes(*commandAmount-1);
        for (int i = 0; i < *commandAmount; i++) { 
            tableauCommand[i] = buildCommandParsed(commandsArgs[i]);
        }
        free(commandsArgs);
        
        // Redirections en cas d'enchainement de pipes        
        int succeed = commandRedirection(tableauCommand, *commandAmount, pipes);
        if (succeed == FALSE) {
            freeCommandArrays(tableauCommand, *commandAmount);
            freePipeArray(pipes, *commandAmount-1);
            free(commandAmount);
            returnValue = 1;
            continue;
        }
        
        for (int i = 0; i < *commandAmount; i++) {       
            command *commande = tableauCommand[i];
            if (commande->arguments->size == 0) {returnValue = 1; break;}
            
            // éxecution de la commande
            commandResult* result = commandProcessHandler(commande, returnValue, last);

            //SA_print(commande->arguments);
            
            if (result->fatal == TRUE) { 
                freeCommand(commande);
                freePipeArray(pipes, *commandAmount-1);
                free(tableauCommand);
                free(commandAmount);
                int exitCode = result->exitCode;
                freeCommandResult(result); 
                return exitCode;
            }
            readResult(commande, result);
            
            if (i==0 && *commandAmount > 1) {
                close(pipes[0][1]);
            } else if (i == *commandAmount-1 && *commandAmount > 1) {
                close(pipes[*commandAmount-2][0]);
            } else if (*commandAmount > 1) {
                close(pipes[i-1][0]);
                close(pipes[i][1]);
            }

            freeCommand(commande);
            returnValue = result->success;
            lastWasSignalTerminated = result->statusExited;
            freeCommandResult(result);
            if(returnValue != 0) break;
        }
        
        freePipeArray(pipes, *commandAmount-1);
        free(commandAmount);
        free(tableauCommand);
    }

    if(prompt != NULL) free(prompt);

    /*étant donné que exit ne fait qu'utiliser la fonction exit(), qui est ici, dans le main, 
    équivalente à un return. Si un utilisateur presse les touche Ctrl+D, il arrivera ici et le
    programme terminera avec la valeur de retour de la dernière commande entrée. Tout comme exit
    sans argument le ferait. */
    return returnValue;
}
