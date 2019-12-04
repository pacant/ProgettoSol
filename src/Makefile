CC = gcc

CFLAGS= -Wall -std=c11

.PHONY: all test clean cleanlog

all: server client supervisor

linkedlist.o: linkedlist.c
	$(CC) $(CFLAGS) -c linkedlist.c -o $@

utility.o: utility.c 
	$(CC) $(CFLAGS) -c utility.c -o $@

supervisor: supervisor.c utility.o linkedlist.o
	$(CC) $(CFLAGS) utility.o linkedlist.o supervisor.c -o $@

server: server.c utility.o
	$(CC) $(CFLAGS) utility.o server.c -o $@ -pthread

client: client.c
	$(CC) $(CFLAGS) client.c -o $@
cleanlog:
	rm -f *.log
clean:
	rm OOB-server-* *.o server supervisor client
test:
	make cleanlog
	make all
	./run.sh
	make clean

