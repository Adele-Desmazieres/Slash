
#include "runner.h"
#include "printing.h"
#include "command/cd.c"
#include "command/pwd.c"
#include "command/exit.c"

//Méthodes de piles de commandes commandStack

stackNode* buildStackNode(command *c){
    stackNode* ret = malloc(sizeof(stackNode));
    if(ret == NULL){
        perror("stackNode malloc error\n");
        return NULL;
    }
    ret->prev = NULL;
    ret->cmd = c;
    return ret;
}

/**
 * @brief Build a new command stack based on the struct \b commandStack.
 * Which contains : The stack length and the top stack node.
 * @return a pointer commandStack* to the commandStack informations.
 */
commandStack* buildCommandStack(){
    commandStack *ret = malloc(sizeof(commandStack));
    if(ret == NULL){
        perror("commandStack malloc error\n");
        return NULL;
    }
    ret->length=0;
    ret->top = NULL;
    return ret;
}

/**
 * @brief Pushes a c command on the s commandStack .
 * @return 0 if successful, -1 otherwise.
 */
int commandStackPush(commandStack* s, command * c){
    stackNode* newTop = buildStackNode(c);
    if(newTop == NULL) return -1;
    newTop->prev = s->top ;
    s->top = newTop;
    s->length++;
    return 0;
}

command* commandStackPop(commandStack* s){
    if (s->length == 0){
        perror("Pop on empty stack");
        return NULL;
    }
    stackNode* oldTop = s->top; 
    command* ret = oldTop->cmd;
    s->top= oldTop->prev;
    free(s->top);
    s->length--;
    return ret;
}

command* commandStackPeek(commandStack* s){
    if(s->length == 0){
        perror("Peek on empty stack");
        return NULL;
    }
    return s->top->cmd;
}

int isEmptyStack(commandStack* s){
    return s->length == 0;
}

int freeStack(commandStack* s){
    while(s->length != 0){
        if(commandStackPop(s) == NULL) return -1;
    }
    free(s);
    return 0;
}

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

    //Création de la pile des commandes
    commandStack* history = buildCommandStack();
    if(history == NULL) {
        return -1;
    }

    char* tmp[2] = {"pwd","-P"}; 
    command* comm = buildCommand(tmp, 2);
    //Ajout à l'historique (exemple)
    commandStackPush(history, comm);
    commandProcessHandler(comm, currPath);

    free(currPath);
    freeStack(history);
}