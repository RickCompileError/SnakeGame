#include "snakeio.h"

Board* initBoard(){
	Board *b = malloc(sizeof(Board));
	b->board_win = newwin(BOARD_ROWS, BOARD_COLS, (LINES-BOARD_ROWS)/2, (COLS-BOARD_COLS)/2);
	b->height = BOARD_ROWS;
	b->width = BOARD_COLS;
	addBoard(b);
	addBoardInfo(b);
    keypad(b->board_win,true);
	wtimeout(b->board_win,SNAKE_MOVE_SPEED);
	return b;
}

void addBoard(const Board *b){
	box(b->board_win,0,0);
}

void addBoardInfo(const Board *b){
	mvwprintw(b->board_win,0,0,"Height: %d, Width: %d",BOARD_ROWS,BOARD_COLS);
}

void addAt(const Board *b, Coordinate c){
	mvwaddch(b->board_win, gety(c), getx(c), getchtype(c));	
}

void getEmptyCoordinates(const Board *b, int *y, int *x){
	while (mvwinch(b->board_win, *y = rand()%b->height+ 1, *x = rand()%b->width+1) != ' ');
}

chtype getInput(const Board *b){
	return wgetch(b->board_win);
}

void clearBoard(const Board *b){
	wclear(b->board_win);
}

void refreshBoard(const Board *b){
	wrefresh(b->board_win);
}
