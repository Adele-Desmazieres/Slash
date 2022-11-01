#ifndef RUNNER_H
#define RUNNER_H

#define TRUE 1
#define FALSE 0
#define PATH_PROMPT_LIMIT 30

#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096

typedef struct command{
    char* name;
    char** args;
    int logicalRef;
    int success;
    int argNumber;
    //targetRef servira pour la commande CD pour stocker le chemin de destination
    char* targetRef;
} command;

command* buildCommand(char** command,  int argNumber);
int commandProcessHandler(command* command, char* currPath);




//Noeud de la pile de commande
typedef struct listNode listNode;
typedef struct listNode{
    command* cmd;
    listNode* prev;
    listNode* next;
} listNode;

//Pile de commande avec le pointeur vers son sommet et sa taille
typedef struct commandList{
    int length;
    listNode* bottom;
    listNode* top;
} commandList;

listNode* buildlistNode(command *c);
commandList* buildCommandList();
int commandListPush(commandList* s, command * c);
command* commandListPop(commandList* s);
command* commandListPeek(commandList* s);
int isEmptyList(commandList* s);
void freeList(commandList* s);

#endif