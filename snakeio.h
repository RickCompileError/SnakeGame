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
#include <arpa/inet.h>
#include <netdb.h>
#include <ncurses.h>
#include <pthread.h>

#include "time.h"

#define BOARD_DIM 35
#define BOARD_ROWS BOARD_DIM
#define BOARD_COLS BOARD_DIM * 3

#define SNAKE_MOVE_SPEED 300

#define MAX_USER 6

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
	int front; // snake head
	int rear; // snake tail
	int sz;
} Snake;

Snake* initSnake();
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
typedef struct{
	Board *board;
	Coordinate *apple;
	bool game_over;
	Snake *snake;
    Snake *snakes[MAX_USER];
    bool server;
} Game;

Game* initGame(bool server);
void startGame(Game *g);
void processInput(Game *g);
void updateState(Game *g);
void redraw(const Game *g);
bool isAppleEat(Coordinate current, Coordinate *apple);
void handleNextMove(Game *g, Coordinate next);
void addSnake(Game *g, int id);
void removeSnake(Game *g, int id);
void setUserDir(Game *g, int id, Direction dir);
bool isOver(const Game *g);
/******************************/

/*********** Apple ************/
void createApple(Game *g);
void deleteApple(Game *g);
int getAppleY(Coordinate *apple);
int getAppleX(Coordinate *apple);
/******************************/

/*********** Package ************/
#define BUF_SIZE 110
typedef enum {
    NEW_CONNECT,
    SNAKE,
    NEW_DIR,
    EAT_APPLE,
    NEW_APPLE,
    USER_DIE
} Package_t;

typedef struct {
    int8_t uid;
    int y;
    int x;
    Direction dir;
    char map[BOARD_COLS];
} GameInfo;

typedef struct {
    Package_t kind;
    GameInfo gi;
} Package;

int recv_package();
int send_package();
/*********** Package ************/

#endif
