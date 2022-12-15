#include "./command.h"

typedef struct Node Node;

typedef struct Node {
	char* content;
	Node* prev;
} Node;

typedef struct Stack {
	Node* head;
	int length;
} Stack;


int isEmpty(Stack* p);
Node* newNode(char* str);
Stack* newStack();
void freeNode(Node* n);
void freeStack(Stack* p);
void push(Stack* p, char* str);
char* peek(Stack* p);
char* pop(Stack* p);
Stack* reverseStack(Stack* s1);