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
typedef struct stackNode stackNode;
typedef struct stackNode{
    command* cmd;
    stackNode* prev;
} stackNode;

//Pile de commande avec le pointeur vers son sommet et sa taille
typedef struct commandStack{
    int length;
    stackNode* top;
} commandStack;

stackNode* buildStackNode(command *c);
commandStack* buildCommandStack();
int commandStackPush(commandStack* s, command * c);
command* commandStackPop(commandStack* s);
command* commandStackPeek(commandStack* s);
int isEmptyStack(commandStack* s);
int freeStack(commandStack* s);

#endif