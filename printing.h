#ifndef PRINTING_H
#define PRINTING_H

void printWithColor(char* message, char* color);
void printSuccess(char* message);
void printError(char* message);
void printCommandExecutionResult(char* message);
void printPrompt(int lastCommandResult, char* path);

#endif