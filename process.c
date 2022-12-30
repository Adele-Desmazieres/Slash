#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>

#include "./process.h"
#include "./command/cd.h"
#include "./command/pwd.h"
#include "./command/exit.h"
#include "./utils/memory.h"
#include "./utils/command.h"
#include "./utils/jokerSimple.h"

void readResult(command* command, commandResult* commandResult) {
    if (commandResult->success == ERROR) {
        if (commandResult->resultMessage) printError(commandResult->resultMessage);
        return;
    }
    if (commandResult->resultMessage) {
        printf("%s\n", commandResult->resultMessage);
    }
}

void fileAttribution(command* command, int mode, char* fileName) {
    int success = FALSE;
    //printf("MODE : -%d-", mode);
    switch (mode) {
        case REDIRECT_INPUT : success = redirectInput(command, open(fileName, O_RDONLY)); break;
        case REDIRECT_OUTPUT : success = redirectOutput(command, open(fileName, O_WRONLY | O_CREAT | O_EXCL, 0644)); break;
        case REDIRECT_OUTPUT_TRUNC : success = redirectOutput(command, open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644)); break;
        case REDIRECT_OUTPUT_CONCAT : success = redirectOutput(command, open(fileName, O_WRONLY | O_CREAT | O_APPEND, 0644)); break;
        case REDIRECT_ERROR : success = redirectErr(command, open(fileName, O_WRONLY | O_CREAT | O_EXCL, 0644)); break;
        case REDIRECT_ERROR_TRUNC : success = redirectErr(command, open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644)); break;
        case REDIRECT_ERROR_CONCAT : success = redirectErr(command, open(fileName, O_WRONLY | O_CREAT | O_APPEND, 0644)); break;
        default: break;
    }

    if (success == FALSE) command->success = FALSE;
}

int destParser(command* command) {
    char* toSearch[7] = {"<", ">", ">|", ">>", "2>", "2>|", "2>>"};
    stringArr* redirectChar = SA_parseArray(toSearch, 7);
    int index = SA_indexOfArray(command->arguments, redirectChar);
    SA_free(redirectChar);
    if (index == -1) return TRUE;
    stringArr* redirection = SA_splice(command->arguments, index, 2);
    command->name = command->arguments->stringArr[0];
    if (redirection->size < 2) { SA_free(redirection); return FALSE; }

    char* fileName = redirection->stringArr[1];
    char* redirectSymbol = redirection->stringArr[0];

    if (strcmp(redirectSymbol, "<") == 0) { fileAttribution(command, REDIRECT_INPUT, fileName); }
    else if (strcmp(redirectSymbol, ">") == 0) { fileAttribution(command, REDIRECT_OUTPUT, fileName); }
    else if (strcmp(redirectSymbol, ">|") == 0) { fileAttribution(command, REDIRECT_OUTPUT_TRUNC, fileName); }
    else if (strcmp(redirectSymbol, ">>") == 0) { fileAttribution(command, REDIRECT_OUTPUT_CONCAT, fileName); }
    else if (strcmp(redirectSymbol, "2>") == 0) { fileAttribution(command, REDIRECT_ERROR, fileName); }
    else if (strcmp(redirectSymbol, "2>|") == 0) { fileAttribution(command, REDIRECT_ERROR_TRUNC, fileName); }
    else if (strcmp(redirectSymbol, "2>>") == 0) { fileAttribution(command, REDIRECT_ERROR_CONCAT, fileName); }
    
    SA_free(redirection);
    if (command->success == FALSE) return FALSE;
    return TRUE;
}

commandResult* startChildCommandProcess(command* command) {
    pid_t r;
    int result;

    switch(r = fork()) {
        case -1: break;
        case 0: 
            dup2(command->redirect.input, STDIN_FILENO);
            dup2(command->redirect.output, STDOUT_FILENO);
            dup2(command->redirect.error, STDERR_FILENO);
            SA_addFinalNull(command->arguments);
            //SA_print(command->arguments);
            execvp(command->name, command->arguments->stringArr);
            exit(127);
        default: 
                waitpid(r, &result, 0);
                int resultStatus = WEXITSTATUS(result);
                int tempReturnValue = (resultStatus == 2 || resultStatus == 15) ? 255 : resultStatus;
                return buildCommandResult(tempReturnValue, NULL);
    }

    //Commande Inconnue.
    return buildCommandResult(FALSE, "Command unknown.\n");
}

commandResult* commandProcessHandler(command* command, int lastCommandState) {   
    // expansion des jokers pour les commandes externes
    stringArr* expanded = expansionJokers(command->arguments);
    alterCommandArgs(command, expanded);
    //Redirections
    if (destParser(command) == FALSE) return buildCommandResult(ERROR, "Redirection Failed.\n");
    //Comande interne
    if ( strcmp(command->name, "exit") == 0 ) return exitCommandRunner(command, lastCommandState);
    if ( strcmp(command->name, "cd") == 0 ) return cdCommandRunner(command);
    if ( strcmp(command->name, "pwd") == 0 ) return pwdCommandRunner(command);
    
    //Commande externe
    return startChildCommandProcess(command);
}


