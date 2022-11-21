#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "./cd.h"
#include "../utils/command.h"
#include "../utils/printing.h"
#include "../utils/stack.h"

#include <stdio.h> // temporaire pour debugger

#ifndef CDC
#define CDC

/* Changement de directory */
commandResult* cdTarget(command* command) {
	char* tmp = malloc(sizeof(char) * (strlen(command->targetRef) + 1));
	if (tmp == NULL) return buildFatalCommandResult(FALSE, "malloc échoué", 1);
	strcpy(tmp, command->targetRef);
	commandResult* res;
	
	int error = 0;
	
	if (chdir(tmp) == 0) {
		
		tmp = realloc(tmp, sizeof(char) * MAX_ARGS_STRLEN);
		if (tmp == NULL) return buildFatalCommandResult(FALSE, "realloc échoué", 1);
		if (getcwd(tmp, sizeof(char) * MAX_ARGS_STRLEN) == NULL) return buildFatalCommandResult(FALSE, "getcwd échoué", 1);
		
		error += setenv("OLDPWD", getenv("PWD"), 1); // maintient le répertoire précédent à jour
		
		// teste si appelé en mode physique ou logique pour savoir quel chemin conserver
		if (command->logicalRef) error += setenv("PWD", command->targetRef, 1); // le chemin donné en argument
		else error += setenv("PWD", tmp, 1); // le chemin renvoyé par getcwd
		
		if (error == 0) return buildCommandResult(TRUE, NULL);
		else res = buildFatalCommandResult(FALSE, "setenv échoué", 1);
		
	} else {
		res = buildCommandResult(FALSE, "Répertoire non trouvé.\n");
	}
	free(tmp);
	return res;
}


/* Set le target directory puis appelle la fonction de chgt de directory. */
commandResult* setTargetToDirectory(command* command, char* target) {
	command->targetRef = malloc(sizeof(char) * (strlen(target) + 1));
	if (command->targetRef == NULL) return buildFatalCommandResult(FALSE, "malloc échoué", 1);
	
	strcpy(command->targetRef, target);
	return cdTarget(command);	
}

void myprint(char *str)
{
    int n = 0;
    do
    {
        printf("str[%d]=", n++);
        switch(*str)
        {
            case '\0':
                printf("`\\0`\n");
                break;
            default:
                if(isprint((unsigned char) *str))
                    printf("'%c'\n", *str);
                else
                    printf("'\\x%02hhx'\n", *str);
        }
    }while(*str++);
}

/* Sépare le string "path" en tokens de string délimités par '/' 
 * place chaque token dans la pile s en argument.
 * Lit dans le sens de lecture si forward=1, dans l'autre sens sinon. */
Stack* split(Stack* s, char* path, int forward) { 
	
	// copie du path en argument pour éviter de le modifier
	char* pathCopy = malloc(sizeof(char) * (strlen(path) + 1));
	if (pathCopy == NULL) perror("malloc\n");
	
	if (path[0] == '/') strcpy(pathCopy, path+1);
	else strcpy(pathCopy, path);
	//strcpy(pathCopy, path);
	
	char* pointeur = strsep(&pathCopy, "/");
	
	while (pointeur != NULL) {
		//myprint(pointeur);
		//printf("%s\n", pointeur);
		push(s, pointeur);
		pointeur = strsep(&pathCopy, "/");
	}
	
	free(pathCopy);
	free(pointeur);

	if (!forward) {
		s = reverseStack(s);
	}
		
	return s;
}

/* CHEMIN LOGIQUE */
/* Construit le chemin du new rep courant, en ajoutant au fur et à mesure les rep du chemin ciblé au chemin actuel. */
char* buildTargetDirectory(command* command, char* relativeLogicalTarget, int relatif) {
	Stack *sCurrent = newStack();
	if (relatif) {
		sCurrent = split(sCurrent, getenv("PWD"), TRUE);
	}

	Stack *sLongTarget = newStack();
	sLongTarget = split(sLongTarget, relativeLogicalTarget, FALSE);
	
	char* tmp;
	char* token;
	//printf(" - empilement - \n");
	
	// rempli la pile du chemin actuel avec les rep du chemin ciblé
	while ((token = pop(sLongTarget))) { // parcourir les tokens du string chemin, séparés par des /
		
		if (strcmp(token, "..") == 0) {
			tmp = pop(sCurrent);
			free(token);
			free(tmp);

		} else if (strcmp(token, ".") == 0) {
			free(token);
			continue;

		} else {
			push(sCurrent, token);
			free(token);
		}
	}
	
	// reconstruit le nouveau chemin actuel en dépilant la pile
	//printf(" - dépilement - \n");
	
	sCurrent = reverseStack(sCurrent);
	char* ret = malloc(sizeof(char) * 2); 
	ret[0] = '/';
	ret[1] = '\0';
	int len = 1;
	
	while (!isEmpty(sCurrent)) {
		
		token = pop(sCurrent);
		//printf("pop : %s\n", token);
		
		ret = realloc(ret, sizeof(char) * (len + strlen(token) + 1));
		if (ret == NULL) perror("realloc\n");
		
		strcpy(ret+len, token);
		len += strlen(token);
		
		if (isEmpty(sCurrent)) {
			ret[len] = '\0';
		} else {
			ret[len] = '/';
		}
		len++;
		
	}
	
	//printf("final : %s\n", ret);
	
	freeStack(sCurrent);
	freeStack(sLongTarget);
	
	return ret;
}


/* CHEMIN LOGIQUE */
// TODO : si cd logique failed, tester en physique
commandResult* cdLogical(command* command, char* target) {
	char first = target[0];
	commandResult* ret;
	
	if (first == '/') { // si commence par un / alors chemin absolu
		ret = setTargetToDirectory(command, buildTargetDirectory(command, target, FALSE));
	
	} else { // sinon on a un chemin relatif
		ret =  setTargetToDirectory(command, buildTargetDirectory(command, target, TRUE));
	}
		
	// si échec en logique, alors cd en physique
	if (!ret->success) {
		command->logicalRef = FALSE;
		ret = setTargetToDirectory(command, target);
	}
	
	return ret;
}



/* Set command->targetRef au répertoire contenu dans la variable d'environnement envVarName */
commandResult* setTargetToEnvVar(command* command, char* envVarName) {
	
	if (getenv(envVarName)) {
		char* buff = malloc(sizeof(char) * (strlen(getenv(envVarName)) + 1) );
		if (buff == NULL) return buildFatalCommandResult(FALSE, "malloc échoué", 1);
		
		strcpy(buff, getenv(envVarName));
		command->targetRef = buff;
		return cdTarget(command);
		
	} else {
		// TODO GERTER LE MALLOC DE YANIS DANS COMMAND.C pour pouvoir malloc tous les mess d'erreur
		// afin d'afficher le nom de la var non initialisée via concaténation
		return buildCommandResult(FALSE, "Variable d'environnement non définie.\n");
	}
}


commandResult* cdCommandRunner(command* command) {
	command->logicalRef = TRUE; // par défaut en mode logique
	commandResult* commandResult;
	
	switch (command->argNumber) {
		// "cd" -> home
		case 1 : 
			commandResult = setTargetToEnvVar(command, "HOME"); 
			command->logicalRef = FALSE;
			break; 
		
		// "cd -" -> OLDPWD OR "cd path/to/directory"
		case 2 : 
			if (strcmp(command->args[1], "-") == 0) {
				command->logicalRef = TRUE;
				commandResult = setTargetToEnvVar(command, "OLDPWD"); 
				
			} else { // cd path/to/directory
				command->logicalRef = TRUE;
				commandResult = cdLogical(command, command->args[1]); // si "cd path" sans option, alors logical
			}
			break;
		
		// "cd [-L | -P] path/to/directory"
		case 3 :
			//command->targetRef = command->args[2];
			if (strcmp(command->args[1], "-L" ) == 0) {
				commandResult = cdLogical(command, command->args[2]); 
			} else if (strcmp(command->args[1], "-P" ) == 0) {
				command->logicalRef = FALSE;
				commandResult = setTargetToDirectory(command, command->args[2]); 
			} else {
				commandResult = buildCommandResult(FALSE, "Invalid argument for the command cd. Expected command format : cd [-L | -P | - ] [ref].\n");
			}
			break;
		
		default :
			commandResult = buildCommandResult(FALSE, "Invalid argument for the command cd. Expected command format : cd [-L | -P | - ] [ref].\n");
		
	}
	return commandResult;
}



#endif