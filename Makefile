.PHONY: all clean
CC = gcc
CFLAGS = -Wall -O3 -g
LDFLAGS = -lncurses

bin = client
obj = game.o client.o board.o coordinate.o snake.o apple.o

all: $(bin) $(obj)

%.o: %.c
	$(CC) -o $@ -c $^ $(CFLAGS)

client: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

client1: client1.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

server: server.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

active_server:
	./server 1111

active_client:
	./client1 127.0.0.1 1111

clean:
	rm $(obj) $(bin)
