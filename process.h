#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>

#include "./command/cd.h"
#include "./command/pwd.h"
#include "./command/exit.h"
#include "./utils/memory.h"
#include "./utils/StringArray.h"
#include "./utils/command.h"
#include "./utils/jokerSimple.h"
#include "./utils/printing.h"

#ifndef PROCESS_H
#define PROCESS_H

#define REDIRECT_INPUT 101
#define REDIRECT_OUTPUT 201
#define REDIRECT_OUTPUT_TRUNC 202
#define REDIRECT_OUTPUT_CONCAT 203
#define REDIRECT_ERROR 301
#define REDIRECT_ERROR_TRUNC 302
#define REDIRECT_ERROR_CONCAT 303

void readResult(command* command, commandResult* commandResult);
void fileAttribution(command* command, int mode, char* fileName);
int destParser(command* command);
commandResult* startChildCommandProcess(command* command);
commandResult* commandProcessHandler(command* command, int lastCommandState);
#endif