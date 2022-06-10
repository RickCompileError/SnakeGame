.PHONY: all clean
CC = gcc
CFLAGS = -Wall -O3 -g
LDFLAGS = -lncurses

bin = start
obj = game.o start.o board.o coordinate.o snake.o apple.o time.o

all: $(bin) $(obj)

%.o: %.c
	$(CC) -o $@ -c $^ $(CFLAGS)

start: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

clean:
	rm $(obj) $(bin)
