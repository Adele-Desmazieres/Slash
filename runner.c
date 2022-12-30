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

int main(int argc, char *argv[]) {
    char* line;
    char* prompt;
    int returnValue = 0;
    
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    //Init. de $PATH
    char* currPathTmp = Malloc(PATH_MAX * sizeof(char) / 2, "Erreur malloc getcwd");
    getcwd(currPathTmp, (PATH_MAX / 2));
    setenv("PWD", currPathTmp, TRUE);
    free(currPathTmp);

    //readline config
    rl_outstream = stderr;
    using_history();

    //Boucle principale
    while (( line = readline((prompt = printPrompt(returnValue, getenv("PWD"))))) != NULL) {
        free(prompt);
        add_history(line);
        //printf("displayed : %s\n", line);        
        
        int* commandAmount = malloc(sizeof(int));
        stringArr* parsed = SA_parseStringWithQuotes(line);
        stringArr** commandsArgs = SA_split(parsed, "|", commandAmount);
        SA_free(parsed);
        
        command** tableauCommand = malloc(sizeof(command) * (*commandAmount));
        int **pipes = malloc(sizeof(int*) * (*commandAmount-1));
        if (pipes == NULL) perror("error malloc");
        
        // transformer le tableau de string en tableau de commandes
        // et malloc les pipes
        //tableauCommand[0] = buildCommandParsed(commandsArgs[0]);
        //pipes[0] = malloc(sizeof(int) * 2);
        //if (pipes[0] == NULL) perror("error malloc");
        
        for (int i = 0; i < *commandAmount; i++) {
            tableauCommand[i] = buildCommandParsed(commandsArgs[i]);
            if (i != *commandAmount - 1) {
                pipes[i] = malloc(sizeof(int) * 2);
                if (pipes[i] == NULL) perror("error malloc");
                if ( pipe(pipes[i]) == -1) perror("error pipe\n");
            }
        }

        // Redirections en cas d'enchainement de pipes        
        for (int i = 0; i < *commandAmount-1; i++) {
            redirectInput(tableauCommand[i+1], pipes[i][0]);
            redirectOutput(tableauCommand[i], pipes[i][1]);
        }
        
        //printf("%d\n", tableauCommand[0]->redirect.output);
        //printf("%d\n", tableauCommand[1]->redirect.input);
        //printf("[%d, %d]\n", pipes[0][0], pipes[0][1]);
        //printf("INPUT1 : %d\n", tableauCommand[0]->redirect.input);
        //printf("OUTPUT1 : %d\n", tableauCommand[0]->redirect.output);
        //printf("INPUT2 : %d\n", tableauCommand[1]->redirect.input);
        //printf("OUTPUT2 : %d\n", tableauCommand[1]->redirect.output);
        
        for (int i = 0; i < *commandAmount; i++) {       
            command *commande = tableauCommand[i];
            if (commande->arguments->size == 0) continue;
            
            // éxecution de la commande
            commandResult* result = commandProcessHandler(commande, returnValue);

            //SA_print(commande->arguments);
            if (i == 0) {
                close(pipes[0][1]);
            } else if (i == *commandAmount-1) {
                close(pipes[*commandAmount-2][0]);
            } else {
                close(pipes[i-1][0]);
                close(pipes[i][1]);
            }
            
            freeCommand(commande);

            if (result->fatal == TRUE) { 
                free(commandsArgs);
                free(commandAmount);
                int exitCode = result->exitCode;
                freeCommandResult(result); 
                free(line);
                return exitCode;
            }
            readResult(commande, result);
            returnValue = result->success;
            freeCommandResult(result);
        }
        
        free(commandsArgs);
        free(commandAmount);
        free(line);
    }

    if(prompt != NULL) free(prompt);

    /*étant donné que exit ne fait qu'utiliser la fonction exit(), qui est ici, dans le main, 
    équivalente à un return. Si un utilisateur presse les touche Ctrl+D, il arrivera ici et le
    programme terminera avec la valeur de retour de la dernière commande entrée. Tout comme exit
    sans argument le ferait. */
    return returnValue;
}
