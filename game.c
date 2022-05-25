#include "snakeio.h"

Game* initGame(){
	initscr();
	noecho();
	curs_set(0);

	box(stdscr,0,0);
	mvprintw(0,0,"Height: %d, Width: %d",LINES,COLS);
	refresh();

	fprintf(fptr,"Sucess initialize stdscr window.\n");

	Game *g = malloc(sizeof(Game));
	g->board = initBoard();
	g->snake = initSnake(right);
	g->apple = NULL;
	g->game_over = false;

	fprintf(fptr,"Sucess initialize Game\n");

	Coordinate next = initCoordinate(3,3,'S');
	addAt(g->board,next);
	addPiece(g->snake,next);

	next = nextHead(g->snake);
	addAt(g->board,next);
	addPiece(g->snake,next);

	next = nextHead(g->snake);
	addAt(g->board,next);
	addPiece(g->snake,next);

	srand(time(NULL));

	return g;
}

void processInput(Game *g){
	chtype input = getInput(g->board);
}

void updateState(Game *g){
	if (g->apple == NULL){
		int y, x;
		getEmptyCoordinates(g->board,&y,&x);
		Coordinate new_apple = initCoordinate(y,x,'A');
		g->apple = &new_apple;
		addAt(g->board,*g->apple);
	}

	Coordinate next = nextHead(g->snake);
	if (gety(next)!=gety(*g->apple) || getx(next)!=getx(*g->apple)){
		int empty_row = gety(tail(g->snake));
		int empty_col = getx(tail(g->snake));
		addAt(g->board,initCoordinate(empty_row,empty_col,' '));
		removePiece(g->snake);
	}
	addAt(g->board,next);
	addPiece(g->snake,next);
}

void redraw(const Game *g){
	refreshBoard(g->board);	
}

void startGame(Game *g){
	while (!isOver(g)){
		processInput(g);
		updateState(g);
		redraw(g);
	}
	getch();
	endwin();
}

bool isOver(const Game *g){
	return g->game_over;
}
