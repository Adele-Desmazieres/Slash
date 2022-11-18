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

/* CHEMIN PHYSIQUE */
commandResult* cdPhysical(command* command) {
	char* newPath = malloc(sizeof(char) * MAX_ARGS_STRLEN);
	
	// si le processus actuel réussi à changer de current working directory
	// et qu'on arrive à récuperer le nouveau directory grace à cwd
	if (chdir(command->targetRef) == 0 && getcwd(newPath, MAX_ARGS_STRLEN)) { 
		
		setenv("OLDPWD", getenv("PWD"), 1); // maintient à jour le rep précédent
		setenv("PWD", newPath, 1); // ici le newPath est le résultat de getcwd
		
		return buildCommandResult(TRUE, newPath); // renvoie une réussite

	} else { // sinon échec du changement de directory
		newPath = realloc(newPath, sizeof(char));
		newPath = "";
		return buildCommandResult(FALSE, newPath); 
	}
}

/* CHEMIN LOGIQUE ABSOLU */
commandResult* cdLogicalAbsolute(command* command) {
	char* newPath = malloc(sizeof(char) * MAX_ARGS_STRLEN);

	if (chdir(command->targetRef) == 0 ) { 
		strcpy(newPath, command->targetRef);
		
		setenv("OLDPWD", getenv("PWD"), 1); 
		setenv("PWD", newPath, 1); // ici le newPath est l'argument de la commande
		
		return buildCommandResult(TRUE, newPath); 

	} else {
		newPath = realloc(newPath, sizeof(char));
		newPath = "";
		return buildCommandResult(FALSE, newPath); 
	}	
}


// split le string "path" par le délimiteur '/' 
// place chaque élément dans la pile s
// dans le sens de lecture si forward=1, dans l'autre sens sinon
Stack* split(Stack* s, const char* path, int forward) { 
	// TODO
	return NULL;
}

/* CHEMIN LOGIQUE RELATIF */
commandResult* cdLogicalRelative(command* command) {
	
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

	return NULL;
}

/* CHEMIN LOGIQUE */
// TODO
commandResult* cdLogical(command* command) {
	char first = command->targetRef[0];
	if (first == '/') { // si commence par un / alors chemin absolu
		return cdLogicalAbsolute(command);
	
	} else { // sinon on a un chemin relatif
		return cdLogicalRelative(command);
	}
}

/* Aller au directory HOME */
commandResult* cdHome(command* command) {
	return NULL;
}

/* Retour au directory précédent */
commandResult* cdBack(command* command) {
	char* tmp;
	
	if ((tmp = getenv("OLDPWD"))) {
		setenv("OLDPWD", getenv("PWD"), 1); 
		setenv("PWD", tmp, 1);
		return buildCommandResult(TRUE, getenv("PWD"));;

	} else {
		return buildCommandResult(FALSE, "OLDPATH non défini.\n");;
	}
}


commandResult* cdCommandRunner(command* command) {
	//cdArgumentHandler(command);	
	//if (command->success == FALSE) return buildCommandResult(FALSE, "");
	commandResult* commandResult;
	
	switch (command->argNumber) {
		case 1 : commandResult = cdHome(command); break; // si "cd" alors -> home
		
		case 2 : 
			if (strcmp(command->args[1], "-") == 0) commandResult = cdBack(command); // si "cd -" alors retour en arriere
			else {
				command->targetRef = command->args[1];
				commandResult = cdLogical(command); // si "cd path" sans option, alors logical
			}
			break;
		
		case 3 :
			command->targetRef = command->args[2];
			if (strcmp(command->args[1], "-L" ) == 0) {
				commandResult = cdLogical(command); 
			} else if (strcmp(command->args[1], "-P" ) == 0) {
				commandResult = cdPhysical(command); 
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