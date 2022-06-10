#include "snakeio.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

Game* initGame(bool server){
	initscr();
	box(stdscr,0,0);
	mvprintw(0,0,"Height: %d, Width: %d",LINES,COLS);
    if (server){
        noecho();
        curs_set(0);
        refresh();
    }

	Game *g = malloc(sizeof(Game));
	g->board = initBoard(server);
	g->apple = NULL;
	g->game_over = false;
    g->server = server;

    createApple(g);

	srand(time(NULL));

	return g;
}

void processInput(Game *g){
	chtype input = getInput(g->board);
    if (g->server) return;
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
    pthread_mutex_lock(&mutex);
    for (int i=0;i<MAX_USER;i++){
        if (g->snakes[i]!=NULL){
            g->snake = g->snakes[i];
            handleNextMove(g,nextHead(g->snakes[i]));
        }
    }
    createApple(g);
    pthread_mutex_unlock(&mutex);
}

void redraw(const Game *g){
	if (g->server) refreshBoard(g->board);	
}

bool isAppleEat(Coordinate current, Coordinate *apple){
    return (apple==NULL) || (gety(current)==gety(*apple) && getx(current)==getx(*apple));
}

void handleNextMove(Game *g, Coordinate next){
    if (g->server) {
        int empty_row = gety(tail(g->snake));
        int empty_col = getx(tail(g->snake));
        addAt(g->board,initCoordinate(empty_row,empty_col,' '));
        removePiece(g->snake);
    } else if (g->apple!=NULL){
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

void addSnake(Game *g, int id){
    g->snakes[id] = initSnake(down);
    Coordinate pos = getStartPosition(g->board);
    g->snake = g->snakes[id];
    handleNextMove(g, pos);
    handleNextMove(g, nextHead(g->snake));
    handleNextMove(g, nextHead(g->snake));
}

void removeSnake(Game *g, int id){
    g->snake = g->snakes[id];
    while (size(g->snake)>0){
        int empty_row = gety(tail(g->snake));
        int empty_col = getx(tail(g->snake));
        addAt(g->board,initCoordinate(empty_row,empty_col,' '));
        removePiece(g->snake);
    }
    free(g->snake);
    g->snakes[id] = NULL;
}

void setUserDir(Game *g, int id, Direction dir){
    setDirection(g->snakes[id], dir);
}

void startGame(Game *g){
	while (g->server || !isOver(g)){
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
