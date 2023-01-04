#ifndef PRINTING_H
#define PRINTING_H 1

#define MAX_PATH_PROMPT_LENGTH 25

void resetPrintColor();
void printWithColor(char* message, char* color);
void printSuccess(char* message);
void printError(char* message);
char* printPrompt(int lastCommandResult, int signalTerminated, char* path);

#endif