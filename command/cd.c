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
		
		setenv("OLDPATH", getenv("PATH"), 1); // maintient à jour le rep précédent
		setenv("PATH", newPath, 1); // ici le newPath est le résultat de getcwd
		
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
		
		setenv("OLDPATH", getenv("PATH"), 1); 
		setenv("PATH", newPath, 1); // ici le newPath est l'argument de la commande
		
		return buildCommandResult(TRUE, newPath); 

	} else {
		newPath = realloc(newPath, sizeof(char));
		newPath = "";
		return buildCommandResult(FALSE, newPath); 
	}	
}

/* CHEMIN LOGIQUE RELATIF */
// split la chaine path par le délimiteur '/' et place chaque élément dans la pile
Stack* split(Stack* s, const char* path) { // renvoie la liste des pointeurs des mots
	// TODO
	return NULL;
}

commandResult* cdLogicalRelative(command* command) {
	// TODO
	/*
	const char* currPath = getenv("PATH");
	char* relatifTargetPath; // TODO : malloc ?
	char* absoluteTargetPath; // TODO : malloc ?s
	strcpy(relatifTargetPath, command->args[2]);
	strcpy(absoluteTargetPath, currPath);

	while(split(currPath)) { // parcourir les tokens du string chemin, séparés par des /
		// TODO
	}
	*/
	return NULL;
}

/* CAS LOGIQUE (-L) */
// TODO
commandResult* cdLogical(command* command) {
	char first = command->targetRef[0];
	if (first == '/') { // si commence par un / alors chemin absolu
		return cdLogicalAbsolute(command);
	
	} else { // sinon on a un chemin relatif
		return cdLogicalRelative(command);
	}
}

commandResult* cdHome(command* command) {
	return NULL;
}

commandResult* cdBack(command* command) {
	return NULL;
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