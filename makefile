CC=gcc
CFLAGS=		-Wall -g -pedantic -fPIC
LDLIBS= 	-lreadline

ALL= runner *.o ./command/*.o ./utils/*.o slash


# RUNNER
slash: utils.o commands.o runner.o
	gcc ./utils/*.o ./command/*.o runner.o -o slash -lreadline

runner.o : runner.c
	gcc -c ./runner.c -o ./runner.o

# COMMANDS
commands.o: ./command/cd.o ./command/exit.o ./command/pwd.o

./command/cd.o: ./command/cd.c
	$(CC) -c -fPIC ./command/cd.c -o ./command/cd.o

./command/exit.o: ./command/exit.c
	$(CC) -c -fPIC ./command/exit.c -o ./command/exit.o

./command/pwd.o: ./command/pwd.c
	$(CC) -c -fPIC ./command/pwd.c -o ./command/pwd.o

# UTILS
utils.o: ./utils/command.o ./utils/lineParser.o ./utils/memory.o ./utils/printing.o ./utils/stack.o

command.o: ./utils/command.c
	$(CC) -c -fPIC ./utils/command.c -o ./utils/command.o

lineParser.o: ./utils/lineParser.c
	$(CC) -c -fPIC ./utils/lineParser.c -o ./utils/lineParser.o

memory.o: ./utils/memory.c
	$(CC) -c -fPIC ./utils/memory.c -o ./utils/memory.o

printing.o: ./utils/printing.c
	$(CC) -c -fPIC ./utils/printing.c -o ./utils/printing.o

stack.o: ./utils/stack.c
	$(CC) -c -fPIC ./utils/stack.c -o ./utils/stack.o



clean:
	rm -rf *.o runner
cleanall:
	rm -rf *~ $(ALL)

cleancommand: 
	rm -rf ./command/*.o runner

cleanutils:
	rm -rf ./utils/*.o runner