#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "runner.h"
#include "printing.h"
#include "command/cd.c"
#include "command/pwd.c"
#include "command/exit.c"

#define PATH_MAX 4096


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
    if ( strcmp(command->name, "exit") == 0 ) return exitCommandRunner(command, currPath);
    if ( strcmp(command->name, "cd") == 0 ) return cdCommandRunner(command, currPath);
    if ( strcmp(command->name, "pwd") == 0 ) return pwdCommandRunner(command, currPath);
    //TO-DO : cas des commandes externes

    //Commande inconnue
    printf("Commande inconnue\n");
    return FALSE;
}

int countArgs(const char* c) {
    
    while(!isalnum(*c)){
        if(*c == '\0') return 0;
        c++;
    } 
    int count = 1;
    while ( *c != '\0'){
        if (*c == ' '){
            count++;
            while (*c == ' '){
                if (*c == '\0') return count;
                c++;
            } 
        }
        c++;
    }
    return count;
}

int sizeOfToken (const char* c){
    int size = 0;
    while(*c != '\0' && *c != ' '){
        size++; c++;
    }
    return size;
}

char** parseLine(const char* line, char** parsedLine) {
    while(!isalnum(*line)){
        if (*line == '\0') return parsedLine;
        line++;
    }
    
    int iterator = 0;
    while(*line != '\0'){
        if(*line != ' '){
            int tokenSize = sizeOfToken(line);
            parsedLine[iterator] = calloc (sizeof(char) , (1 + tokenSize));
            strncat(parsedLine[iterator], line, tokenSize);
            parsedLine[iterator++][tokenSize] = '\0';
            line += tokenSize;
            if(*line == '\0') return parsedLine; 
        }
        line++;
    }
    
    return parsedLine;
}

void printParsed(char ** parsed, int len){
    for(int i = 0; i < len; i++){
        printf(" argument %d : %s \n", i+1, parsed[i]);
    }
}

int main(int argc, char *argv[]) {
    //char* path = "oulalah/cest/trop/long/osskour/aled/je/meurs";
    //char* newPath = reducePathPromptLenght(path);
    //printf("%s", newPath);

    //printPrompt(0, path);
    //printf("\n");
    
    rl_outstream = stderr; // affichage du prompt sur la sortie erreur, voir sujet

    char* line = malloc(sizeof(char) * MAX_ARGS_STRLEN);
    if (line == NULL) { return -1; }

    char* currPath = malloc((PATH_MAX / 4) * sizeof(char)); // a mettre à jour avec chaque appel à cd ; PATH_MAX / 4 car PATH_MAX / 2 trop grand
    getcwd(currPath, (PATH_MAX / 4));
    if (currPath == NULL) {
        perror("problème repertoire courant"); return -1;
    }

    using_history();
    int returnValue = 0; 

    while ((line = readline(printPrompt(returnValue,currPath))) != NULL) {
        char** parsedLine = NULL;
        add_history(line); 

        printf("displayed : %s\n", line);

        // parser la ligne (mots et opérateurs séparés par des espaces)
        int nbrArgs = countArgs(line);
        if (nbrArgs == 0) continue;
        //printf(" > nbrArgs : %d\n", nbrArgs);

        parsedLine = calloc(nbrArgs , sizeof(char *));
        if (parsedLine == NULL) perror ("Erreur calloc");
        
        //if (parsedLine == NULL) { return -1; }
        
        parsedLine = parseLine(line, parsedLine);
        printf("-----------\n");
        printParsed(parsedLine, nbrArgs);        
        printf("-----------\n");
        
        // initier un objet commande
        command* commande = buildCommand(parsedLine, nbrArgs);
        

        // interpréter la commande
        int stop = commandProcessHandler(commande, currPath);
        
        freeParsedLine(parsedLine, nbrArgs);
        free(commande);
    }

    /* OLD TESTS
    char* tmp[2] = {"pwd","-P"}; 
    command* comm = buildCommand(tmp, 2);
    commandProcessHandler(comm, currPath);
    readline( printPrompt(0, path) );
    */
    
    return returnValue;
}