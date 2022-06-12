.PHONY: all clean
CC = gcc
CFLAGS = -Wall -O3 -g
LDFLAGS = -lncurses -pthread

bin = server client
obj = game.o board.o coordinate.o snake.o apple.o time.o package.o

all: $(bin) $(obj)

%.o: %.c
	$(CC) -o $@ -c $^ $(CFLAGS)

test: $(bin)

server: $(obj) server.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

client: $(obj) client.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

active_server:
	./server 1111 2> servlog.txt

active_client:
	./client 127.0.0.1 1111 2> clilog.txt

clean:
	rm $(obj) $(bin) client.o server.o
