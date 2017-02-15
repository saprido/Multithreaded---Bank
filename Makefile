CC = gcc
CF = -Wall -Werror

all: client server

client: client.c
	$(CC)  -o client client.c

server: bank.o server.o
	$(CC) $(CF) -o server bank.o server.o -lpthread

testserver.o: server.c bank.h
	$(CC) -c server.c -g

bank.o: bank.c bank.h
	$(CC) $(CF) -c bank.c -g

clean:
	rm -f *~
	rm -f *.o
	rm -f server
	rm -f client