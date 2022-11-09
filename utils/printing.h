#ifndef PRINTING_H
#define PRINTING_H 1

void printWithColor(char* message, char* color);
void printSuccess(char* message);
void printError(char* message);
void printCommandExecutionResult(char* message);
char* printPrompt(int lastCommandResult, char* path);

#endif