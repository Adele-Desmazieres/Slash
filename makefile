CC=gcc
CFLAGS=		-Wall -g -pedantic
LDLIBS= 	-lreadline

ALL= runner
all: $(ALL)


runner: printing.o runner.o

printing.o : printing.c printing.h
runner.o: runner.c printing.h command/pwd.c command/exit.c command/cd.c



clean:
	rm -rf *.o runner
cleanall:
	rm -rf *~ *.o $(ALL)