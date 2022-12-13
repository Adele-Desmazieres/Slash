#ifndef JOKER_SIMPLE_H
#define JOKER_SIMPLE_H

typedef struct listNode listNode;

typedef struct listNode {
    char* content;
    listNode* next;
} listNode;

typedef struct pathList {
    int len;
    listNode* first;
} pathList;

int containsSimpleJoker (const char* path);
void freepathList(pathList* arr);
int isSuffix(const char* str, const char* suffix);
char* getSuffix(const char* path);
pathList* jokerSimple(char* orPath);
char** pathListToArray(pathList* p);
char** expansionJokers(char** args, int len, int* newLen);


#endif