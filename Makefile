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

clean:
	rm $(obj) $(bin)
