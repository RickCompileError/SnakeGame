#include "snakeio.h"

Game* initGame(){
	initscr();
	noecho();
	curs_set(0);

	box(stdscr,0,0);
	mvprintw(0,0,"Height: %d, Width: %d",LINES,COLS);
	refresh();

	Game *g = malloc(sizeof(Game));
	g->board = initBoard();
	g->snake = initSnake(right);
	g->apple = NULL;
	g->game_over = false;

    handleNextMove(g,initCoordinate(3,3,'S'));
    handleNextMove(g,nextHead(g->snake));
    handleNextMove(g,nextHead(g->snake));

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
    handleNextMove(g,nextHead(g->snake));
    createApple(g);
}

void redraw(const Game *g){
	refreshBoard(g->board);	
}

bool isAppleEat(Coordinate current, Coordinate *apple){
    return (apple==NULL) || (gety(current)==gety(*apple) && getx(current)==getx(*apple));
}

void handleNextMove(Game *g, Coordinate next){
    if (g->apple!=NULL){
        switch (getAt(g->board, next)){
            case 'A':
                deleteApple(g);
                break;
            case ' ':; // C language only allows statements to follow a label
                int empty_row = gety(tail(g->snake));
                int empty_col = getx(tail(g->snake));
                addAt(g->board,initCoordinate(empty_row,empty_col,' '));
                removePiece(g->snake);
                break;
            default:
                g->game_over = true;
                break;
        }
    }
    addAt(g->board,next);
	addPiece(g->snake,next);
}

void startGame(Game *g){
	while (!isOver(g)){
		processInput(g);
		updateState(g);
		redraw(g);
	}
	// getch();
	endwin();
}

bool isOver(const Game *g){
	return g->game_over;
}
