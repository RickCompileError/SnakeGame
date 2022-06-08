#ifndef SNACKIO_H
#define SNACKIO_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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
Coordinate head();
Coordinate tail();
Coordinate nextHead();
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
void getEmptyCoordinates(const Board *b, int *y, int *x);
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
    bool show;
} Game;

Game* initGame(bool show);
void startGame(Game *g);
void processInput(Game *g);
void updateState(Game *g);
void redraw(const Game *g);
bool isAppleEat(Coordinate current, Coordinate *apple);
void handleNextMove(Game *g, Coordinate next);
bool isOver(const Game *g);
/******************************/

/*********** Apple ************/
void createApple(Game *g);
void deleteApple(Game *g);
/******************************/

/*********** Package ************/
#define BUF_SIZE 10
typedef enum {
    NEW_CONNECT,
    GET_POS,
    RET_POS,
    NEW_DIR,
    EAT_APPLE,
    NEW_APPLE,
    USER_DIE
} Package_t;

typedef struct {
    Package_t package_t;
    char data[BUF_SIZE];
} Package;

void handle_package();
void prepare_package();
/*********** Package ************/

#endif
