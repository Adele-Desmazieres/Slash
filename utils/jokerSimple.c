#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>       
#include <fcntl.h>
#include <errno.h>

#include "jokerSimple.h"
#include "StringArray.h"
//#include "lineParser.h"


void printParsedJoker(char ** parsed, int len){
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
    if(strcmp(str, ".") == 0 && strcmp(suffix, "..") == 0) return 0;
    if(strcmp(str, "..") == 0 && strcmp(suffix, ".") == 0) return 0;
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
            char* tmp = malloc(sizeof(char)); tmp[0] = '\0'; 
            return tmp;
        }


        char* tmp = malloc(sizeof(char) * (strlen(path)+1));
        if(tmp == NULL) perror("Erreur malloc getSuffix ");
        strcpy(tmp,path);

        char* ret = malloc(sizeof(char) * (strlen(path)+1));
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
    
    listNode* tmp = arr->first;

    while (tmp != NULL)
    {
        free(tmp->content);
        listNode* toFree = tmp;
        tmp = tmp->next;
        free(toFree);


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
    while(tmp->next != NULL) {
        tmp = tmp->next;
    }
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


int countArgsJoker(const char* c) {
   
    while(*c == '/'){
        if(*c == '\0') return 0;
        c++;
    }

    int count = 1;
    int len = strlen(c);
    for (int i = 0; i < len-1; i++) {
        if (c[i] == '/' && c[i+1] != '\0' ) count++;
    }
    
    return count;
}


int sizeOfTokenJoker(const char* c){
    int size = 0;
    while(*c != '\0'){
        size++; 
        if(*c == '/') return size;
        c++;
    }
    return size;
}

char** parseLineJoker(const char* line, char** parsedLine) {
    while(*line == '/'){
        if (*line == '\0') return parsedLine;
        line++;
    }
    int iterator = 0;
    while(*line != '\0'){
        //if(*line != '/'){
            int tokenSize = sizeOfTokenJoker(line);
            //if(line[tokenSize-1] == '/') tokenSize--;
            parsedLine[iterator] = calloc ((1 + tokenSize), sizeof(char));
            strncat(parsedLine[iterator], line, tokenSize);
            parsedLine[iterator][tokenSize] = '\0';
            iterator++;
            line += tokenSize;
            if(*line == '\0') return parsedLine; 
        //}
        if (tokenSize == 0) line++;
    }
    return parsedLine;
}

void freeParsedLineJoker(char** parsedLine, int parseLineLength) {
    for (int i = 0; i < parseLineLength; i++) {
        if(parsedLine[i]) free(parsedLine[i]);
    }
    free(parsedLine);
}

char* deleteCurrDirOnPath(char* path) {
    int len = strlen(path);
    if(len == 0) return path;
    path[len-1] = '\0';
    while(path[len-1] != '/') {
        path[len-1] = '\0';
        len--;
    }
    if (len <= 0) return path;
    return path;
}

char* mergeFilePathAndName(const char* path, char* filename, int isDir) {
    char* filePath = malloc((strlen(path)+2+strlen(filename)+1) * sizeof(char));
    if(filePath == NULL) perror("erreur malloc");
    strcpy(filePath, path);
    strcat(filePath, filename);
    if (isDir && strcmp(filePath, "./") != 0) strcat(filePath, "/");

    return filePath;
}

void parcourirRepertoire (pathList* p, int depth, int maxDepth, const char* currPath, char** pathArray){
    if(depth == maxDepth) return;
    //printf("PP : %s\n", currPath);
    struct dirent* de;
    DIR* dir;
    struct stat st;

    char * pathIsHere = "./";
    char* currPathCpy = malloc((strlen(currPath)+1) * sizeof(char));
    if(currPathCpy == NULL) perror("erreur malloc");
    strcpy(currPathCpy, currPath);
    //printf("nom du chemin courant : %s\n", currPathCpy);
    
    char* currentPredicat = pathArray[depth];
    while ((depth < maxDepth-1) && strcmp(pathArray[depth+1], "/") == 0) {
        depth++;
        //printf("Current : %d/%d\n", depth, maxDepth);
    }
    
    //Trouver suffixe du répertoire courant s'il existe, ouvrir le bon repertoire sinon
    char* suffixe = getSuffix(currentPredicat);

    //Si on rencontre . on ne change pas le dossier de recherche
    //Si on rencontre .. on remonte dans l'arborescence;
    if(strcmp(suffixe, "./") == 0) {
        char* merged = mergeFilePathAndName(currPath, ".", 1);
        parcourirRepertoire(p, depth+1, maxDepth, merged, pathArray);
        free(merged);
        free(currPathCpy);
        free(suffixe);
        return;
    }
    if(strcmp(suffixe, "../") == 0) {
        char* merged = mergeFilePathAndName(currPath, "..", 1);
        parcourirRepertoire(p, depth+1, maxDepth, merged, pathArray);
        free(merged);
        free(currPathCpy);
        free(suffixe);
        return;
    }

    //printf("Suffixe courant = %s\n", suffixe);
    int searchDir = 0;
    int suffixeSize = strlen(suffixe);
    if (suffixeSize != 0 && suffixe[suffixeSize-1] == '/') {
        searchDir = 1;
        suffixe[suffixeSize-1] = '\0';
    }
    //printf("Suffixe courant après modif bizarre = %s \n", suffixe);
    //if (strcmp("*", suffixe) == 0) allRepertoire = 1;
    //printf("nom du suffixe courant : %s\n", suffixe);
    //printf("AllRepertoire = %d \n", allRepertoire);

    //printf("Profondeur courante = %d ; Profondeur max = %d ; Condition de terminaison? %d \n\n\n", depth, maxDepth, (depth >= maxDepth - 1));
    //Si depth = maxDepth - 1, alors ajouter à p tous les fichiers qui correspond au suffixe courant

    //Si on ne peut pas ouvrir le répertoire, alors on abandonne cet appel == répertoire inexistant ou pas accès
    char* pathToOpen = (strcmp(currPathCpy, "") == 0) ? pathIsHere : currPathCpy;
    if ((dir = opendir(pathToOpen)) == NULL){
        //printf("itération sur %s échouée\n", currPathCpy);
        free(suffixe);
        free(currPathCpy);
        return;
    }
    
    while ((de = readdir(dir)) != NULL){
        //On ignore tout les dossiers cachés
        //Si c'est . et que depth = max-1 passe
        //Sinon si c'est .. et que depth = max-1 passe
        //Sinon si ça commence par . ça passe pas
        if(de->d_name[0] == '.') continue;

        int pathSize = (strlen(pathToOpen) + strlen(de->d_name) +1);
        char* currFilePath = malloc(pathSize * sizeof(char));
        snprintf(currFilePath, pathSize, "%s%s", pathToOpen, de->d_name);
        //printf("SEARCHING : %s\n", currFilePath);
        if(stat(currFilePath, &st) < 0) perror(" erreur stat() : jokerSimple");
        free(currFilePath);
        if (searchDir && !S_ISDIR(st.st_mode)) continue;
        //printf("NAME %s SERCH %d and ISFILE %d (%d)\n", de->d_name, searchDir, !S_ISDIR(st.st_mode), st.st_ino);
        //On récupère les infos du fichier courant
        //printf("%s is suffix : %d\n", de->d_name, isSuffix(de->d_name, suffixe));
        if ( isSuffix(de->d_name, suffixe) ){
            //printf("IsSuffix(. ..) %d \n", isSuffix(".", ".."));
            //On fait une copie locale du chemin
            char* basePath = (strcmp(pathToOpen, "./") == 0) ? "" : pathToOpen;
            if (strcmp(currPath, "./") == 0) basePath = "./";
            char* currPathCpy2 = mergeFilePathAndName(basePath, de->d_name, S_ISDIR(st.st_mode));
            //printf("MATCHING : %s\n", currPathCpy2);
            //On ajoute le nom du fichier courant à la copie du path
            //printf("test à l'ajout\n");
            if(depth == maxDepth-1) {
                //On ajoute ce nom à la liste
                int tmpLen = strlen(currPathCpy2)-1;
                if (!searchDir && currPathCpy2[tmpLen] == '/') currPathCpy2[tmpLen] = '\0';
                ajouterPath(p,currPathCpy2);
            }else {
                parcourirRepertoire(p, depth+1, maxDepth, currPathCpy2, pathArray);
            }
            free(currPathCpy2);
        }
    }
    closedir(dir);

    //Libération de la mémoire
    free(suffixe);
    free(currPathCpy);
}


void parcoursDouble(pathList* p, int maxDepth, const char* currPath, char** pathArray, int start){

    struct dirent* de;
    DIR* dir;
    struct stat st;


    char* currPathCpy = malloc((strlen(currPath)+1) * sizeof(char));
    if(currPathCpy == NULL) perror("erreur malloc");
    strcpy(currPathCpy, currPath);

    //printf("nom du chemin courant : %s\n", currPathCpy);

    //On parcourt le répertoire courant
    if(start == 1){
        if( (dir = opendir("./")) == NULL){
            free(currPathCpy);
            return;
        }
        while ((de = readdir(dir)) != NULL ){
            //On passe . et ..
            if( de->d_name[0] == '.') continue;
                char* currPathCpy2 = malloc((strlen(currPath)+2+strlen(de->d_name)+1) * sizeof(char));
                if(currPathCpy2 == NULL) perror("erreur malloc");
                strcpy(currPathCpy2, currPathCpy);
                strcat(currPathCpy2, de->d_name);
                //On récupère les infos fichier
                if( (lstat("./", &st) < 0)) perror(" erreur stat() : jokerSimple");
                strcat(currPathCpy2, "/");
                //printf("Nom du chemin recherché : %s \n", currPathCpy2);
                if(S_ISDIR(st.st_mode)) parcourirRepertoire(p, 0, maxDepth, currPathCpy2, pathArray);
                int fd;
                if ((fd = open(currPathCpy2, O_NOFOLLOW) < 0)){
                    if (errno == ELOOP) printf("Youpi \n");
                    else parcoursDouble(p, maxDepth, currPathCpy2, pathArray, 0);
                } else if(de->d_type != DT_LNK && !S_ISLNK(st.st_mode)) parcoursDouble(p, maxDepth, currPathCpy2, pathArray, 0);
                free(currPathCpy2);
    }
    closedir(dir);        

    } else {
        if ((dir = opendir(currPathCpy)) == NULL){
                //printf("itération sur %s échouée\n", currPathCpy);
                free(currPathCpy);
                return;
        }
        while ((de = readdir(dir)) != NULL ){
            //On passe . et ..
            if( de->d_name[0] == '.') continue;
                char* currPathCpy2 = malloc((strlen(currPath)+2+strlen(de->d_name)+1) * sizeof(char));
                if(currPathCpy2 == NULL) perror("erreur malloc");
                strcpy(currPathCpy2, currPathCpy);
                strcat(currPathCpy2, de->d_name);
                //On récupère les infos fichier
                if( (lstat("./", &st) < 0)) perror(" erreur stat() : jokerSimple");
                strcat(currPathCpy2, "/");
                //printf("Nom du chemin recherché : %s \n", currPathCpy2);
                if(S_ISDIR(st.st_mode)) parcourirRepertoire(p, 0, maxDepth, currPathCpy2, pathArray);
                int fd;
                if ((fd = open(currPathCpy2, O_NOFOLLOW) < 0)){
                    if (errno == ELOOP) {}
                    else parcoursDouble(p, maxDepth, currPathCpy2, pathArray, 0);
                } else if(de->d_type != DT_LNK && !S_ISLNK(st.st_mode)) parcoursDouble(p, maxDepth, currPathCpy2, pathArray, 0);
                free(currPathCpy2);
    }
    closedir(dir);      
    }
      

    //Libération de la mémoire
    free(currPathCpy);
}


pathList* jokerSimple(char* orPath){

    if (strcmp(orPath, "**") == 0) return jokerSimple("**/*");
    if (strcmp(orPath, "**/") == 0) return jokerSimple("**/*/");

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
        memmove(orPathCpy, orPathCpy+3, strlen(orPathCpy) - 2); doubleJoker = 1;
    }
    //Comptage des arguments pour le tableau
    int maxDepth = countArgsJoker(orPathCpy);
    
    char* pathCpy;


    //Init. du tableau d'arguments (chemins coupés par /)
    char** args = malloc(sizeof(char *) * maxDepth);
    if(args == NULL) perror("erreur malloc");
    args = parseLineJoker(orPathCpy, args);
    //printf("nombre d'arguments / profondeur du chemin = %d\n", maxDepth);

    //Chemin absolu ou relatif
    if(*(orPathCpy) == '/') pathCpy = "/";
    else pathCpy = "";

    //printf("%s\n",pathCpy);
    //printParsedJoker(args, maxDepth);
    //Appel initial sur le répertoire au début du chemin
    if(doubleJoker == 0) {
        parcourirRepertoire(ret, 0, maxDepth, pathCpy, args);
    }else {
        parcourirRepertoire(ret, 0, maxDepth, pathCpy, args);
        parcoursDouble(ret, maxDepth, pathCpy, args, 1);
    }
    //printParsedJoker(args, maxDepth);
    freeParsedLineJoker(args, maxDepth);
    //if(doubleJoker) orPathCpy -= 3;
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


stringArr* pathListToArray(pathList* p) {
    stringArr* ret = createStringArray();

    int i = 0;
    listNode* tmp = p->first;
    while(i < p->len){
        SA_add(ret, tmp->content);

        i++; tmp = tmp->next;
    }

    return ret;
}


// Prend en argument un tableau de string représentant les arguments de la commande
// Renvoie un tableau de string des arguments, dont les joker ont été interprêtés
stringArr* expansionJokers(stringArr* args) {
    pathList* listeDesArgs = creerPathList();
    int nbArgs = 0;
    int i = 0;
    
    int len = args->size;
    while (i < len) {
        
        char* argument = args->stringArr[i];
        if (containsSimpleJoker(argument)) {
            //printf("Test for args : %s\n", argument);
            pathList* tmp = jokerSimple(argument);
            listeDesArgs = concatList(listeDesArgs, tmp);
            nbArgs += tmp->len;
            if (tmp->len == 0) {
                ajouterPath(listeDesArgs, argument);
                nbArgs++;
            }
            freepathList(tmp);
        } else {
            ajouterPath(listeDesArgs, argument);
            nbArgs += 1;
        }
        i++;
    }
    stringArr* ret = pathListToArray(listeDesArgs);
    freepathList(listeDesArgs);
    
    return ret;
    
}



/*
int main(void){

    printf("\n\n    ESSAI PARSER : \n");

    char* testPath = "/helloo/foo/lol/luzog/test/essai/oui";
    char** test1 = malloc(sizeof(char * ) * 7);
    if(test1 == NULL) perror("malloc test");
    test1 = parseLineJoker(testPath, test1);

    printf("Nombre d'arguments : %d\n", countArgsJoker(testPath));
    printParsedJoker(test1, 7);

    freeParsedLineJoker(test1, 7);

    printf("\n\n\n");

    pathList* testList = creerPathList();
    ajouterPath(testList,"haha");
    ajouterPath(testList, "hoho");
    ajouterPath(testList, "huhu");

    afficherPathList(testList);
    freepathList(testList);

    printf("\n\n\n");

    char* testPathAbs = "/ * / *";

    pathList* p = jokerSimple(testPathAbs);

    //afficherPathList(p);

    char** essaiconv = pathListToArray(p);
    //printParsedJoker(essaiconv, p->len);

    freeParsedLineJoker(essaiconv, p->len);
    freepathList(p);

    printf("\n\n\n");

    pathList* p1 = jokerSimple("** /  *");

    afficherPathList(p1); 
    freepathList(p1); 
    
    // TESTS 2

    char* a = "echo";
    char* b = "-P";
    char* c = "example";
    char* d = "*";
    
    char* in1[] = {a, b, c, d};
    printParsedJoker(in1, 4);
    printf("\n");
    
    int* newLen = malloc(sizeof(int));
    if (newLen == NULL) exit(-1);
    char** out1 = expansionJokers(in1, 4, newLen);
    printf("%d\n", *newLen);
    printParsedJoker(out1, *newLen);
    printf("\n");
    
}
*/





