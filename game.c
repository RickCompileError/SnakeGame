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

    createApple(g);

	srand(time(NULL));

	return g;
}

void processInput(Game *g){
	chtype input = getInput(g->board);
    switch (input){
        case 'w':
        case KEY_UP:
            setDirection(g->snake,up);
            break;
        case 'a':
        case KEY_LEFT:
            setDirection(g->snake,left);
            break;
        case 's':
        case KEY_DOWN:
            setDirection(g->snake,down);
            break;
        case 'd':
        case KEY_RIGHT:
            setDirection(g->snake,right);
            break;
        default:
            break;
    }
}

void updateState(Game *g){
	Coordinate next = nextHead(g->snake);
    nextMove(g,next);
    createApple(g);
	addAt(g->board,next);
	addPiece(g->snake,next);
}

void redraw(const Game *g){
	refreshBoard(g->board);	
}

bool isAppleEat(Coordinate current, Coordinate apple){
    return gety(current)==gety(apple) && getx(current)==getx(apple);
}

void nextMove(Game *g, Coordinate next){
    if (!isAppleEat(next,*g->apple)){
		int empty_row = gety(tail(g->snake));
		int empty_col = getx(tail(g->snake));
		addAt(g->board,initCoordinate(empty_row,empty_col,' '));
		removePiece(g->snake);
	}
    else deleteApple(g);
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
