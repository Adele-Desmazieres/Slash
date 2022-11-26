#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include "jokerSimple.h"

int containsSimpleJoker (const char* path){
    while(*path != '\0'){
        if(*path == '/' && *(path+1) != '\0' && *(path+1) == '*' ) return 1;
        path++;
    }
    return 0;
}


int isSuffix(const char* str, const char* suffix){
    int minLen = (strlen(str) > strlen(suffix)) ? strlen(suffix) : strlen(str);
    str += strlen(str) - 1;
    suffix += strlen(suffix) - 1;
    int i = 0;
    while (minLen > 0){
        if (*(suffix - i) != *(str - i)) return 0;
        i++;
        minLen--;
    }

    return 1;
}

char* truncatePath(const char* path){

    int acc = 0;
    char* tmp = malloc(sizeof(char) * (strlen(path)+1));
    if(tmp == NULL) perror("Erreur malloc truncatePath() ");
    strcpy(tmp,path);
    while(*tmp != '\0' && *(tmp+1) != '\0' && !(*tmp == '/' && *(tmp+1) == '*' ) ){
        acc++; tmp++;
    }

    if(*tmp == '\0' || *(tmp+1) == '\0') { free(tmp); return NULL; };

    tmp -= acc;

    int len = strlen(tmp);

    if (len > abs(acc)) {
        tmp[len-acc] = 0;
    }

    return tmp;
}

char* getSuffix(const char* path){

    int acc = 0;
    char* tmp = malloc(sizeof(char) * (strlen(path)+1));
    if(tmp == NULL) perror("Erreur malloc truncatePath() ");
    strcpy(tmp,path);
    while(*tmp != '\0' && *(tmp+1) != '\0' && !(*tmp == '/' && *(tmp+1) == '*' ) ){
        acc++; tmp++;
    }

    if(*tmp == '\0' || *(tmp+1) == '\0') { free(tmp); return NULL; }
    if (*(tmp+1) == '*' && *(tmp+2) == '\0'){
        free(tmp);
        return "*";
    }

    tmp -= acc;
    acc+=2;

    int len = strlen(tmp);

    if (len > abs(acc)) {
        if(acc > 0){
            tmp = tmp + acc;
        } else {
        tmp[len + acc] = 0;
        }
    }

    return tmp;
}

//Renvoie un tableau de String contenant les chemins vers tous les fichiers qui sont étendus par le joker contenu dans str
//Renvoie NULL si pas de joker detecté dans str
pathArray* jokerSimple(const char* str){

    int all = 0;

    // tronquer le chemin pour avoir le parent de la destination ! déjà Malloc !
    char* truncPath = truncatePath(str);
    if(truncatePath == NULL) return NULL;
    // trouver le suffixe voulu ! déjà Malloc !
    char* suffix = getSuffix(str);
    if(suffix == NULL) return NULL;
    if (strcmp(suffix, "*") == 0) all = 1;

    pathArray* arr = malloc(sizeof(pathArray));
    if(arr == NULL) perror("Malloc pathArray");

    int nbrOfEntries = 0;

    struct dirent* de;
    DIR* dir;
    if ((dir = opendir(truncPath)) == NULL) return NULL;
    while ((de = readdir(dir)) != NULL ){
        if ( strcmp(de->d_name, ".") != 0 && strcmp(de->d_name,"..") != 0 && (all || isSuffix(de->d_name, suffix)) ) nbrOfEntries++;
    }
    rewinddir(dir);

    arr->len = nbrOfEntries;

    char** ret = malloc(sizeof(char *) * nbrOfEntries);
    if(ret == NULL) perror("malloc joker simple 1");
    int iterator = 0;

    while ((de = readdir(dir)) != NULL ){
        if ( de->d_name[0]!='.' && (all || isSuffix(de->d_name, suffix)) ){
            ret[iterator] = malloc (sizeof(char) * (1+ strlen( strcat(truncPath, de->d_name))) );
            if (ret[iterator] == NULL) perror ("malloc joker simple 2");
            ret[iterator++] = strcat(truncPath, de->d_name); 
        }
    }
    closedir(dir);

    if (all == 0) free(suffix);
    free(truncPath);

    arr->content = ret;

    return arr;

}

void freePathArray(pathArray* arr){
    for(int i = 0; i < arr->len; i++){
        free(arr->content[i]);
    }
    free(arr->content);

    free(arr);
}
