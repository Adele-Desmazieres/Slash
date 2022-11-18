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

// TODO : malloc les chaines de char dans les messages d'erreur ?
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
		
		error += setenv("OLDPWD", getenv("PWD"), 1);
		
		// teste si appelé en mode physique ou logique pour savoir quel path renvoyer
		if (command->logicalRef) error += setenv("PWD", command->targetRef, 1); // le chemin donné en argument
		else error += setenv("PWD", tmp, 1); // le chemin renvoyé par getcwd
		
		if (error == 0) return buildCommandResult(TRUE, tmp);
		else res = buildFatalCommandResult(FALSE, "setenv échoué", 1);
		
	} else {
		res = buildCommandResult(FALSE, "Répertoire non trouvé.\n");
	}
	free(tmp);
	return res;
}

/* CHEMIN PHYSIQUE ET CHEMIN LOGIQUE ABSOLU 
 * Set leur target directory puis appelle la fonction de chgt de directory. */
commandResult* setTargetToDirectory(command* command, char* target) {
	command->targetRef = malloc(sizeof(char) * (strlen(target) + 1));
	if (command->targetRef == NULL) return buildFatalCommandResult(FALSE, "malloc échoué", 1);
	
	strcpy(command->targetRef, target);
	return cdTarget(command);	
}


/* Sépare le string "path" par le délimiteur '/' 
 * place chaque élément dans la pile s
 * dans le sens de lecture si forward=1, dans l'autre sens sinon */
Stack* split(Stack* s, const char* path, int forward) { 
	// TODO
	return NULL;
}

/* CHEMIN LOGIQUE RELATIF */
commandResult* cdLogicalRelative(command* command, char* target) {
	/*
	const char* currPath = getenv("PWD");
	char* absoluteTargetPath; // TODO : malloc ?
	strcpy(absoluteTargetPath, currPath);
	Stack* s1 = newStack();
	s1 = split(s1, absoluteTargetPath, TRUE);
	
	char* relatifTargetPath; // TODO : malloc ?
	strcpy(relatifTargetPath, command->args[2]);
	Stack* s2 = newStack();
	s2 = split(s2, relatifTargetPath, FALSE);
	
	char* token;
	
	while((token = pop(s2))) { // parcourir les tokens du string chemin, séparés par des /
		// TODO
	}
	*/
	return NULL;
}


/* CHEMIN LOGIQUE */
commandResult* cdLogical(command* command, char* target) {
	
	char first = target[0];
	if (first == '/') { // si commence par un / alors chemin absolu
		return setTargetToDirectory(command, target);
	
	} else { // sinon on a un chemin relatif
		return cdLogicalRelative(command, target);
	}
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
		// TODO GERTER LE MALLOC DE YANIS DANS COMMAND.C pour pouvoir malloc ici le mess d'erreur
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
				command->logicalRef = FALSE;
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