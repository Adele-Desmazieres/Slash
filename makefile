CC=gcc
CFLAGS=		-Wall -g -pedantic
LDLIBS= 	-lm

ALL= runner
all: $(ALL)


runner: printing.o runner.o

printing.o : printing.c printing.h
runner.o: runner.c printing.h command/pwd.c command/exit.c command/cd.c



clean:
	rm -rf *~
cleanall:
	rm -rf *~ *.o $(ALL)