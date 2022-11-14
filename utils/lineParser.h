#ifndef LINE_PARSER_H
#define LINE_PARSER_H 1

int countArgs(const char *c);
int sizeOfToken(const char* c);
char** parseLine(const char* line, char** parsedLine);
void printParsed(char ** parsed, int len);
void freeParsedLine(char** parsedLine, int parseLineLength);

#endif