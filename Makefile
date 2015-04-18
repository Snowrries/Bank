CC=gcc
CCFLAGS= -Wall -g -Werror -lpthread 
OBJECTS = Bank.o client.o server.o

all:  $(OBJECTS)
	$(CC) $(CCFLAGS) Bank.o server.o -o server
	$(CC) $(CCFLAGS) client.o  -o client

Bank.o: Bank.c Bank.h 
	$(CC) $(CCFLAGS) -c Bank.c

server.o: server.c server.h
	$(CC) $(CCFLAGS) -c server.c

client.o: client.c client.h
	$(CC) $(CCFLAGS) -c client.c

.PHONY : clean
clean:
	rm -f *.o
	rm server
	rm client

