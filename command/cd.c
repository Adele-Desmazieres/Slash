#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "./cd.h"
#include "../utils/command.h"
#include "../utils/memory.h"
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
		free(tmp);
		tmp = malloc(sizeof(char) * MAX_ARGS_STRLEN);
		if (tmp == NULL) return buildFatalCommandResult(ERROR, "realloc échoué", 1);
		if (getcwd(tmp, sizeof(char) * MAX_ARGS_STRLEN) == NULL) {
			free(tmp);
			return buildFatalCommandResult(ERROR, "getcwd échoué", 1);
		}
		
		error += setenv("OLDPWD", getenv("PWD"), 1); // maintient le répertoire précédent à jour
		
		// teste si appelé en mode physique ou logique pour savoir quel chemin conserver
		if (command->logicalRef) error += setenv("PWD", command->targetRef, 1); // le chemin donné en argument
		else error += setenv("PWD", tmp, 1); // le chemin renvoyé par getcwd
		
		if (error == 0) {
			free(tmp);
			return buildCommandResult(SUCCESS, NULL);
		}
		else res = buildFatalCommandResult(ERROR, "setenv échoué", 1);
		
	} else {
		res = buildCommandResult(ERROR, "Répertoire non trouvé.\n");
	}
	free(tmp);
	return res;
}


/* Set le target directory puis appelle la fonction de chgt de directory. */
commandResult* setTargetToDirectory(command* command, char* target) {
	if (command->targetRef) free(command->targetRef);
	command->targetRef = malloc(sizeof(char) * (strlen(target) + 1));
	if (command->targetRef == NULL) {
		free(target);
		return buildFatalCommandResult(ERROR, "malloc échoué", 1);
	}
	strcpy(command->targetRef, target);
	free(target);
	return cdTarget(command);	
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
	
	char* tmp = pathCopy;
	char* pointeur = strsep(&tmp, "/");
	
	while (pointeur != NULL) {
		//printf("%s\n", pointeur);
		push(s, pointeur);
		pointeur = strsep(&tmp, "/");
	}
	free(pathCopy);
	if (!forward) s = reverseStack(s);
		
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
			free(tmp);

		} else if (strcmp(token, ".") == 0) {
			continue;

		} else {
			push(sCurrent, token);
		}

		free(token);
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
		free(token);

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
	if (ret->success) {
		command->logicalRef = FALSE;
		freeCommandResult(ret);
		ret = setTargetToDirectory(command, target);
	}else {
		free(target);
	}
	
	return ret;
}



/* Set command->targetRef au répertoire contenu dans la variable d'environnement envVarName */
commandResult* setTargetToEnvVar(command* command, char* envVarName) {
	
	if (getenv(envVarName)) {
		char* buff = malloc(sizeof(char) * (strlen(getenv(envVarName)) + 1) );
		if (buff == NULL) return buildFatalCommandResult(ERROR, "malloc échoué", 1);
		
		strcpy(buff, getenv(envVarName));
		command->targetRef = buff;
		return cdTarget(command);
		
	} else {
		// TODO GERTER LE MALLOC DE YANIS DANS COMMAND.C pour pouvoir malloc tous les mess d'erreur
		// afin d'afficher le nom de la var non initialisée via concaténation
		return buildCommandResult(ERROR, "Variable d'environnement non définie.\n");
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
				char* tmp = Malloc(strlen(command->args[1])*sizeof(char)+1, "");
				strcpy(tmp, command->args[1]);
				commandResult = cdLogical(command, tmp); // si "cd path" sans option, alors logical
			}
			break;
		
		// "cd [-L | -P] path/to/directory"
		case 3 :
			//command->targetRef = command->args[2];
			if (strcmp(command->args[1], "-L" ) == 0) {
				char* tmp = Malloc(strlen(command->args[2])*sizeof(char)+1, "");
				strcpy(tmp, command->args[2]);
				commandResult = cdLogical(command, tmp); 
			} else if (strcmp(command->args[1], "-P" ) == 0) {
				command->logicalRef = FALSE;
				char* tmp = Malloc(strlen(command->args[2])*sizeof(char)+1, "");
				strcpy(tmp, command->args[2]);
				commandResult = setTargetToDirectory(command, tmp);
			} else {
				commandResult = buildCommandResult(ERROR, "Invalid argument for the command cd. Expected command format : cd [-L | -P | - ] [ref].\n");
			}
			break;
		
		default :
			commandResult = buildCommandResult(ERROR, "Invalid argument for the command cd. Expected command format : cd [-L | -P | - ] [ref].\n");
		
	}
	return commandResult;
}



#endif
