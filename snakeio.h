#ifndef SNACKIO_H
#define SNACKIO_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ncurses.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

#include "time.h"

#define BOARD_DIM 20
#define BOARD_ROWS BOARD_DIM
#define BOARD_COLS BOARD_DIM * 3

#define SNAKE_MOVE_SPEED 800

#define MAX_USER 6
#define MAX_FD 100

pthread_mutex_t lock;

/*********** Coordiante *************/
typedef struct{
	int x;
	int y;
	chtype ch;
} Coordinate;

Coordinate initCoordinate();
int gety(Coordinate c);
int getx(Coordinate c);
chtype getchtype(Coordinate c);
/************************************/

/*********** Snake *************/
typedef enum{
	up = -1,
	down = 1,
	left = -2,
	right = 2
} Direction;

#define QUEUE_SIZE (size_t)4000
typedef struct{
	Direction direction;
	Coordinate *snake_piece;
    chtype skin;
	int front; // snake head
	int rear; // snake tail
	int sz;
} Snake;

Snake* initSnake(Direction dir);
void addPiece(Snake *s, Coordinate c);
void removePiece(Snake *s);
Direction getDirection(Snake *s);
void setDirection(Snake *s, Direction dir);
Coordinate head(Snake *s);
Coordinate tail(Snake *s);
Coordinate nextHead(Snake *s);
int size(Snake *s);
/*******************************/

/************ Board Structure *************/
typedef struct{
	WINDOW *board_win;
	int height;
	int width;
} Board;

Board* initBoard();
void addBoard(const Board *b);
void addBoardInfo(const Board *b);
void addGameInfo(const Board *b, char *info);
void addAt(const Board *b, Coordinate c);
chtype getAt(const Board *b, Coordinate c);
chtype getAtYX(const Board *b, int y, int x);
void getStr(const Board *b, int y, int x, char *str);
void setStr(const Board *b, int y, int x, char *str);
void getEmptyCoordinates(const Board *b, int *y, int *x);
Coordinate getStartPosition(const Board *b);
chtype getInput(const Board *b);
void clearBoard(const Board *b);
void refreshBoard(const Board *b);
void setTimeout(WINDOW *win, int timeout);
/*****************************************/

/*********** Game *************/
typedef enum{
    SERVER,
    CLIENT
} Type;

typedef struct {
    int8_t uid;
    int y;
    int x;
    Direction dir;
    char map[BOARD_COLS];
} GameInfo;

typedef struct{
	Board *board;
	Coordinate *apple;
	bool game_over;
	Snake *snake;
    Snake *snakes[MAX_USER];
    Type type;
    int id;
    int fd;
    int snakeEat;
} Game;

Game* initGame(Type type);
void startGame(Game *g);
void processInput(Game *g);
void updateState(Game *g);
void redraw(const Game *g);
void handleNextMove(Game *g, Coordinate next, bool self);
void serverAddSnake(Game *g, int id);
void dfs(Snake *snake, Board *board, int prevy, int prevx, int cury, int curx, chtype ch);
void clientAddSnake(Game *g, GameInfo gi, bool isNewSnake);
void showGameInfo(Game *g);
void removeSnake(Game *g, int id);
void setUserDir(Game *g, int id, Direction dir);
bool isOver(const Game *g);
/******************************/

/*********** Apple ************/
void createApple(Game *g);
void addApple(Game *g, Coordinate new_apple);
void cleanApple(Game *g);
void deleteApple(Game *g);
void setApple(Coordinate *apple, int y, int x);
int getAppleY(Coordinate *apple);
int getAppleX(Coordinate *apple);
/******************************/

/*********** Package ************/
#define BUF_SIZE 110
typedef enum {
    SET_ID=1,
    SET_MAP,
    SET_SNAKE,
    NEW_SNAKE,
    NEW_DIR,
    EAT_APPLE,
    NEW_APPLE,
    USER_DIE
} Package_t;

typedef struct {
    Package_t type;
    GameInfo gi;
} Package;

int recv_package();
int send_package();
void sendDirection(int fd, int8_t uid, Direction dir);
void sendEatApple(int fd, int8_t uid);
void sendDie(int fd, int8_t uid);
/*********** Package ************/

#endif
