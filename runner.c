
#include "runner.h"
#include "printing.h"
#include "command/cd.c"
#include "command/pwd.c"
#include "command/exit.c"

//Méthodes de listes de commandes commandList

listNode* buildlistNode(command *c){
    listNode* ret = malloc(sizeof(listNode));
    if(ret == NULL){
        perror("listNode malloc error\n");
        return NULL;
    }
    ret->prev = NULL;
    ret->next = NULL;
    ret->cmd = c;
    return ret;
}

/**
 * @brief Build a new command List based on the struct \b commandList.
 * Which contains : The List length and the top List node.
 * @return a pointer commandList* to the commandList informations.
 */
commandList* buildCommandList(){
    commandList *ret = malloc(sizeof(commandList));
    if(ret == NULL){
        perror("commandList malloc error\n");
        return NULL;
    }
    ret->length=0;
    ret->top = NULL;
    ret->bottom = NULL;
    return ret;
}

/**
 * @brief Pushes a c command on the s commandList .
 * @return 0 if successful, -1 otherwise.
 */
int commandListPush(commandList* s, command * c){
    listNode* newTop = buildlistNode(c);
    if(newTop == NULL) return -1;

    switch (s->length)
    {
    case 0 : 
        s->top = newTop;
        s->bottom = newTop;
        s->length++;
        return 0;
    
    default:
        newTop->prev = s->top;
        s->top->next = newTop;
        s->top = newTop;
        s->length++;
        return 0;
    }

    return -1;
}

/**
 * @brief gets the top command of the command list .
 * @return a command* located at the top of the list or NULL if the list is empty .
 */
command* commandListPeek(commandList* s){
    if(s->length == 0){
        perror("Peek on empty List");
        return NULL;
    }
    return s->top->cmd;
}

int isEmptyList(commandList* s){
    return s->length == 0;
}

void freeList(commandList* s){

    listNode* tmp = s->bottom;
    while(s->length != 0 && tmp != NULL){
        free(tmp->cmd);
        free(tmp->prev);
        tmp=tmp->next;
    }
    free(s->top);
    free(s->bottom);
    free(s);
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
    commandList* history = buildCommandList();
    if(history == NULL) {
        return -1;
    }

    char* tmp[2] = {"pwd","-P"}; 
    command* comm = buildCommand(tmp, 2);
    //Ajout à l'historique (exemple)
    commandListPush(history, comm);
    commandProcessHandler(comm, currPath);

    free(currPath);
    freeList(history);
}