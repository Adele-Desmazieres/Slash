CC=gcc
CFLAGS=		-Wall -g -pedantic
LDLIBS= 	-lreadline

ALL= runner
all: $(ALL)


runner: printing.o command.o runner.o lineParser.o

printing.o : utils/printing.c utils/printing.h
command.o : utils/command.c utils/command.h
lineParser.o : utils/lineParser.c utils/lineParser.h
runner.o: runner.c utils/printing.h utils/command.h utils/lineParser.h command/pwd.c command/exit.c command/cd.c



clean:
	rm -rf *.o runner
cleanall:
	rm -rf *~ *.o $(ALL)