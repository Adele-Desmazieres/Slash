#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stack.h"
#include "memory.h"

int isEmpty(Stack* p) {
	return p->length == 0;
}

Node* newNode(char* str) {
	Node* ret = Malloc(sizeof(Node), "1");
	
	ret->content = malloc(sizeof(char) * (strlen(str) + 1) );
	if (ret->content == NULL) {
		free(ret);
		perror("2");
	}

	strcpy(ret->content, str);
	ret->prev = NULL;
	return ret;
}

Stack* newStack() {
	Stack* ret = malloc(sizeof(Stack));
	if (ret == NULL) perror("3");
	ret->head = NULL;
	ret->length = 0;
	return ret;
}

void freeNode(Node* n) {
	free(n->content);
	free(n);
	return;
}

void freeStack(Stack* p) {
	Node* head = p->head;
	while (head != NULL) {
		Node* tmp = head;
		head = head->prev;
		freeNode(tmp);
	}
	free(p);
	return;
}

void push(Stack* p, char* str) {
	Node* n = newNode(str);
	//free(str);
	if (p->length == 0){
		p->head = n;
		p->length++; 
	} else {
		n->prev = p->head;
		p->head = n;
		p->length++;
	}
	return;
}

char* peek(Stack* p) {
	if (isEmpty(p)) return NULL;
	return p->head->content;
}

char* pop(Stack* p) {
	if (isEmpty(p)) return NULL;
	
	Node* n = p->head;
	char* ret = n->content;
	p->length--;
	if (p->length == 0) {
		p->head = NULL; 
	} else {
		p->head = n->prev;
	}
	free(n);
	return ret;
}

Stack* reverseStack(Stack* s1) {
	Stack* s2 = newStack();
	char* pointeur;
	while (!isEmpty(s1)) {
		pointeur = pop(s1);
		push(s2, pointeur);
		free(pointeur);
	}
	freeStack(s1);
	return s2;
}

