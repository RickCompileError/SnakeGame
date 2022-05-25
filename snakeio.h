#ifndef SNACKIO_H
#define SNACKIO_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>

#define BOARD_DIM 35
#define BOARD_ROWS BOARD_DIM
#define BOARD_COLS BOARD_DIM * 3

#define SNAKE_MOVE_SPEED 200

#define MAX_USER 4

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
	up,
	down,
	left,
	right
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
void getEmptyCoordinates(const Board *b, int *y, int *x);
chtype getInput(const Board *b);
void clearBoard(const Board *b);
void refreshBoard(const Board *b);
/*****************************************/

/*********** Game *************/
typedef struct{
	Board *board;
	Snake *snake;
	Coordinate *apple;
	bool game_over;
} Game;
Game* initGame();
void startGame(Game *g);
void processInput(Game *g);
void updateState(Game *g);
void redraw(const Game *g);
bool isOver(const Game *g);
/******************************/

/************ LOG *************/
FILE *fptr;
/******************************/
#endif
