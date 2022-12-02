#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include "jokerSimple.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void printParsed(char ** parsed, int len){
    for(int i = 0; i < len; i++) printf(" argument %d : %s \n", i+1, parsed[i]);
}

int containsSimpleJoker (const char* path){
    int i = 0;
    while(path[i] != '\0'){
        if(path[i] == '*' ) return 1;
        i++;
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

char* getSuffix(const char* path){

    if(!containsSimpleJoker(path)){
        char* ret = malloc(sizeof(char)*(strlen(path)+1));
        if(ret == NULL) perror("malloc @ getSuffix equals");
        strcpy(ret,path);
        return ret;
    }else{

        if(path[strlen(path) - 1] == '*'){
            char* tmp = malloc(sizeof(char) * 2); tmp[0] = '*'; tmp[1] = '\0'; return tmp;
        }

        char* tmp = malloc(sizeof(char) * (strlen(path)+1));
        if(tmp == NULL) perror("Erreur malloc getSuffix ");
        strcpy(tmp,path);

        char* ret = malloc(sizeof(char) * (strlen(path)));
        if(ret == NULL) perror("Erreur malloc getSuffix ");
        strcpy(ret,path);

        int i = 1;
        while(tmp[i] != '\0'){
            ret[i-1] = tmp[i];
            i++;
        }

        ret[i-1] = '\0';


        free(tmp);
        return ret;
    
    }

    
}


void freepathList(pathList* arr){
    for(listNode* tmp = arr->first; tmp != NULL;){
        free(tmp->content);
        listNode* tmp2 = tmp;
        tmp = tmp->next;
        free(tmp2);
    }

    free(arr);
}

pathList* creerPathList(){
    pathList* ret = malloc(sizeof(pathList));
    if(ret == NULL) perror("Erreur malloc creerListe : jokerSimple");
    ret->len = 0;
    ret->first = NULL;
    return ret;
}

listNode* creerListNode(const char* path){
    listNode* ret = malloc(sizeof(listNode));
    if(ret == NULL) perror("Erreur malloc creerListe : jokerSimple");
    ret->next = NULL;
    ret->content = malloc(sizeof(char) * (strlen(path) + 1));
    if(ret->content == NULL) perror("Erreur malloc string creer listNode : jokerSimple");
    strcpy(ret->content, path);
    return ret;
}

void ajouterPath (pathList* p, const char* path){
    listNode* nouv = creerListNode(path);
    if(p->len == 0){
        p->first = nouv;
        p->len++;
        return;
    }
    listNode* tmp = p->first;
    while(tmp->next != NULL) tmp = tmp->next;
    tmp->next = nouv;
    p->len++;
    return;
    
}

pathList* concatList(pathList* dest, pathList* src){
    listNode* tmp = src->first;
    while(tmp != NULL){
        ajouterPath(dest, tmp->content);
        tmp = tmp->next;
    }
    return dest;
}


int countArgs(const char* c) {
    
    while(*c == '/'){
        if(*c == '\0') return 0;
        c++;
    } 
    int count = 1;
    while ( *c != '\0'){
        if (*c == '/'){
            count++;
            while (*c == '/'){
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
    while(*c != '\0' && *c != '/'){
        size++; c++;
    }
    return size;
}

char** parseLine(const char* line, char** parsedLine) {
    while(*line == '/'){
        if (*line == '\0') return parsedLine;
        line++;
    }
    
    int iterator = 0;
    while(*line != '\0'){
        if(*line != '/'){
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

void freeParsedLine(char** parsedLine, int parseLineLength) {
    for (int i = 0; i < parseLineLength; i++) {
        free(parsedLine[i]);
    }
    free(parsedLine);
}

void parcourirRepertoire (pathList* p, int depth, int maxDepth, const char* currPath, char** pathArray, int start, int doubles){

    struct dirent* de;
    DIR* dir;
    struct stat st;

    char* currPathCpy = malloc((strlen(currPath)+1) * sizeof(char));
    if(currPathCpy == NULL) perror("erreur malloc");
    strcpy(currPathCpy, currPath);
    //printf("nom du chemin courant : %s\n", currPathCpy);
    
    int allRepertoire = 0;

    //Trouver suffixe du répertoire courant s'il existe, ouvrir le bon repertoire sinon
    char* suffixe = getSuffix(pathArray[depth]);
    if (strcmp("*", suffixe) == 0) allRepertoire = 1;
    //printf("nom du suffixe courant : %s\n", suffixe);


    //Si depth = maxDepth - 1, alors ajouter à p tous les fichiers qui correspond au suffixe courant
    if(depth >= maxDepth - 1){
        //Si on ne peut pas ouvrir le répertoire, alors on abandonne cet appel == répertoire inexistant ou pas accès
        if ((dir = opendir(currPathCpy)) == NULL){
            //printf("itération sur %s échouée\n", currPathCpy);
            free(suffixe);
            free(currPathCpy);
            return;
        } 
        while ((de = readdir(dir)) != NULL ){
            //On passe . et ..
            if( de->d_name[0] == '.') continue;
            if ((allRepertoire || isSuffix(de->d_name, suffixe))){
                //On fait une copie locale du chemin
                //printf("Fichier: %s \n", de->d_name);
                char* currPathCpy2 = malloc((strlen(currPath)+2+strlen(de->d_name)+1) * sizeof(char));
                if(currPathCpy2 == NULL) perror("erreur malloc");
                strcpy(currPathCpy2, currPathCpy);
                if (start == 0 || doubles == 1) strcat(currPathCpy2, "/");
                //On ajoute le nom du fichier courant à la copie du path
                strcat(currPathCpy2, de->d_name);
                //On ajoute ce nom à la liste
                //printf("test à l'ajout\n");
                ajouterPath(p,currPathCpy2);
                free(currPathCpy2);
            }
        }
        closedir(dir);
    } else {
        
        //Ouvrir répertoire courant, parcourir, et si il existe un fichier qui correspond au suffixe / nom alors appel récursif dessus
        //NE PAS OUBLIER DE METTRE A JOUR LE REPERTOIRE COURANT SUR UNE ITERATION

        //Si on ne peut pas ouvrir le répertoire, alors on abandonne cet appel == répertoire inexistant ou pas accès
        if ((dir = opendir(currPathCpy)) == NULL){
            //printf("itération sur %s échouée\n", currPathCpy);
            free(suffixe);
            free(currPathCpy);
            return;
        }
        while ((de = readdir(dir)) != NULL ){
            //On passe . et ..
            if( de->d_name[0] == '.') continue;
            //On récupère les infos du fichier courant
            if( (stat(currPathCpy, &st) < 0)) perror(" erreur stat() : jokerSimple");
            //printf("%d test %s\n",isSuffix(de->d_name, suffix), de->d_name);
            //Si (le nom correspond au suffixe OU l'étoile est complète) et qu'il ne s'agit pas d'un lien symbolique et qu'il s'agit d'un répertoire
            if (S_ISDIR(st.st_mode) && !(S_ISLNK(st.st_mode)) && (allRepertoire || isSuffix(de->d_name, suffixe)) ){
                //On fait une copie locale du chemin
                char* currPathCpy2 = malloc((strlen(currPath)+2+strlen(de->d_name)+1) * sizeof(char));
                if(currPathCpy2 == NULL) perror("erreur malloc");
                strcpy(currPathCpy2, currPathCpy);
                if(start == 0) strcat(currPathCpy2, "/");
                //On ajoute le nom du répertoire courant à la copie du path
                strcat(currPathCpy2, de->d_name);
                //appel récursif sur ce répertoire
                //printf("passage dans : %s\n", currPathCpy2);
                parcourirRepertoire(p, depth+1, maxDepth, currPathCpy2, pathArray, 0, 0);
                free(currPathCpy2);
            }
        }
        closedir(dir);        
    }

    //Libération de la mémoire
    free(suffixe);
    free(currPathCpy);
}


void parcoursDouble(pathList* p, int depth, int maxDepth, const char* currPath, char** pathArray, int start){

    struct dirent* de;
    DIR* dir;
    struct stat st;

    char* currPathCpy = malloc((strlen(currPath)+1) * sizeof(char));
    if(currPathCpy == NULL) perror("erreur malloc");
    strcpy(currPathCpy, currPath);
    //printf("nom du chemin courant : %s\n", currPathCpy);
    
    int allRepertoire = 0;

    //Trouver suffixe du répertoire courant s'il existe, ouvrir le bon repertoire sinon
    char* suffixe = getSuffix(pathArray[0]);
    if (strcmp("*", suffixe) == 0) allRepertoire = 1;
    //printf("nom du suffixe courant : %s\n", suffixe);

    //On parcourt le répertoire courant
    if ((dir = opendir(currPathCpy)) == NULL){
            //printf("itération sur %s échouée\n", currPathCpy);
            free(suffixe);
            free(currPathCpy);
            return;
    }
    while ((de = readdir(dir)) != NULL ){
            //On passe . et ..
            if( de->d_name[0] == '.') continue;
            //On récupère les infos du fichier courant
            if( (stat(currPathCpy, &st) < 0)) perror(" erreur stat() : jokerSimple");
            if(S_ISDIR(st.st_mode) && !(S_ISLNK(st.st_mode)) ){
                char* currPathCpy2 = malloc((strlen(currPath)+2+strlen(de->d_name)+1) * sizeof(char));
                if(currPathCpy2 == NULL) perror("erreur malloc");
                strcpy(currPathCpy2, currPathCpy);
                if(start == 0) strcat(currPathCpy2, "/");
                strcat(currPathCpy2, de->d_name);
                parcourirRepertoire(p, 0, maxDepth, currPathCpy2, pathArray, 1, 1);
                parcoursDouble(p, 0, maxDepth, currPathCpy2, pathArray, 0);
                free(currPathCpy2);
            }
    }
    closedir(dir);        

    //Libération de la mémoire
    free(suffixe);
    free(currPathCpy);
}


pathList* jokerSimple(char* orPath){

    char* orPathCpy = malloc(sizeof(char) * (strlen(orPath)+1));
    if (orPathCpy == NULL) perror("Malloc orPathCpy @ jokerSimple");
    strcpy(orPathCpy, orPath);

    pathList* ret = creerPathList();
    int doubleJoker = 0;

    if( !containsSimpleJoker(orPathCpy) ) {
        ajouterPath(ret, orPathCpy);
        return ret;
    }

    //Detection du joker double et avancée
    if(strlen(orPathCpy) >= 3 && orPath[0] == '*' && orPath[1] == '*' && orPath[2] == '/'){
        orPathCpy += 3; doubleJoker = 1;
    }

    //Comptage des arguments pour le tableau
    int maxDepth = countArgs(orPathCpy);
    
    char* pathCpy;


    //Init. du tableau d'arguments (chemins coupés par /)
    char** args = malloc(sizeof(char *) * maxDepth);
    if(args == NULL) perror("erreur malloc");
    args = parseLine(orPathCpy, args);
    //printf("%d\n", maxDepth);

    //Chemin absolu ou relatif
    if(*(orPathCpy) == '/') pathCpy = "/";
    else pathCpy = "./";

    //printf("%s\n",pathCpy);

    //printParsed(args, maxDepth);
    //Appel initial sur le répertoire au début du chemin
    if(doubleJoker == 0) parcourirRepertoire (ret, 0, maxDepth, pathCpy, args, 1, 0);
    else {
        parcourirRepertoire (ret, 0, maxDepth, pathCpy, args, 1, 0);
        parcoursDouble(ret, 0, maxDepth, pathCpy, args, 1);
    }
    //printf("test2\n");
    freeParsedLine(args, maxDepth);
    if(doubleJoker) orPathCpy -= 3;
    free(orPathCpy);

    return ret;
}

void afficherPathList(pathList* p){
    printf(" Affichage d'une liste de chemins\n");
    listNode* tmp = p->first;

    while(tmp != NULL){
        printf("  %s\n",tmp->content);
        tmp = tmp->next;
    }

}


char** pathListToArray(pathList* p){
    char** ret = malloc(sizeof(char * )* p->len);
    if(ret == NULL) perror("Erreur malloc @ pathListToArray : jokerSimple");

    int i = 0;
    listNode* tmp = p->first;
    while(i < p->len){
        ret[i] = malloc(sizeof(char) * (strlen(tmp->content)+1));
        if(ret[i] == NULL) perror("Erreur malloc @ pathListToArray string : jokerSimple");
        strcpy(ret[i], tmp->content);

        i++; tmp = tmp->next;
    }

    return ret;
}




/*
int main(void){


    printf("\n\n    ESSAI PARSER : \n");

    char* testPath = "/helloo/foo/lol/luzog/test/essai/oui";
    char** test1 = malloc(sizeof(char * ) * 7);
    if(test1 == NULL) perror("malloc test");
    test1 = parseLine(testPath, test1);

    printf("Nombre d'arguments : %d\n", countArgs(testPath));
    printParsed(test1, 7);

    freeParsedLine(test1, 7);

    printf("\n\n\n");

    pathList* testList = creerPathList();
    ajouterPath(testList,"haha");
    ajouterPath(testList, "hoho");
    ajouterPath(testList, "huhu");

    afficherPathList(testList);
    freepathList(testList);

    printf("\n\n\n");

    char* testPathAbs = "/* / *";

    pathList* p = jokerSimple(testPathAbs);

    //afficherPathList(p);

    char** essaiconv = pathListToArray(p);
    //printParsed(essaiconv, p->len);

    freeParsedLine(essaiconv, p->len);
    freepathList(p);

    printf("\n\n\n");

    pathList* p1 = jokerSimple("** /  *");

    afficherPathList(p1); 
    freepathList(p1); 

}
*/





