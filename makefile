CC=gcc
CFLAGS=		-Wall -g -pedantic
LDLIBS= 	-lreadline

ALL= runner
all: $(ALL)

# RUNNER
runner:
	$(CC) $(CFLAGS) ./utils/*.c ./command/*.c ./runner.c -o runner $(LDLIBS)


clean:
	rm -rf *.o runner
cleanall:
	rm -rf *~ $(ALL)