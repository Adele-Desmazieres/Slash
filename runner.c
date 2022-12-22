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
        
        for (int i = 0; i < *commandAmount; i++) {       
            // initier un objet commande puis interprète la commande
            command* commande = buildCommandParsed(commandsArgs[i]);
            //SA_free(commandsArgs[i]);
            if (commande->arguments->size == 0) continue;

            //SA_print(commande->arguments);
            commandResult* result = commandProcessHandler(commande, returnValue);

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
