#include "snakeio.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

Game* initGame(Type type){
	initscr();
	box(stdscr,0,0);
	mvprintw(0,0,"Height: %d, Width: %d",LINES,COLS);
    if (true){
        noecho();
        curs_set(0);
        refresh();
    }

	Game *g = malloc(sizeof(Game));
	g->board = initBoard();
	g->apple = NULL;
	g->game_over = false;
    g->snake = NULL;
    for (int i=0;i<MAX_USER;i++) g->snakes[i] = NULL;
    g->type = type;
    g->id = g->fd = -1;
    if (g->type==SERVER) createApple(g);

	srand(time(NULL));

    fprintf(stderr, "[Game] Create Game Success\n");

	return g;
}

void processInput(Game *g){
	chtype input = getInput(g->board);
    if (g->type==SERVER || (g->type==CLIENT && g->id==-1)) return;
    pthread_mutex_lock(&lock);
    switch (input){
        case 'w':
        case KEY_UP:
            setDirection(g->snake,up);
            sendDirection(g->fd, g->id, up);
            break;
        case 'a':
        case KEY_LEFT:
            setDirection(g->snake,left);
            sendDirection(g->fd, g->id, left);
            break;
        case 's':
        case KEY_DOWN:
            setDirection(g->snake,down);
            sendDirection(g->fd, g->id, down);
            break;
        case 'd':
        case KEY_RIGHT:
            setDirection(g->snake,right);
            sendDirection(g->fd, g->id, right);
            break;
        default:
            break;
    }
    pthread_mutex_unlock(&lock);
}

void updateState(Game *g){
    pthread_mutex_lock(&lock);
    for (int i=0;i<MAX_USER;i++){
        if (g->snakes[i]!=NULL){
            g->snake = g->snakes[i];
            handleNextMove(g, nextHead(g->snake), i==g->id);
        }
    }
    if (g->id!=-1) g->snake = g->snakes[g->id];
    pthread_mutex_unlock(&lock);
}

void redraw(const Game *g){
    refreshBoard(g->board);	
}

void handleNextMove(Game *g, Coordinate next, bool self){
    if (g->type==SERVER || !self) {
        int empty_row = gety(tail(g->snake));
        int empty_col = getx(tail(g->snake));
        addAt(g->board,initCoordinate(empty_row,empty_col,' '));
        if (!g->isAppleEat) removePiece(g->snake);
        else g->isAppleEat = false;
    } else {
        switch (getAt(g->board, next)){
            case 'A':
                sendEatApple(g->fd, g->id);
                break;
            case ' ':; // C language only allows statements to follow a label
                int empty_row = gety(tail(g->snake));
                int empty_col = getx(tail(g->snake));
                addAt(g->board,initCoordinate(empty_row,empty_col,' '));
                removePiece(g->snake);
                break;
            default:
                g->game_over = true;
                sendDie(g->fd, g->id);
                break;
        }
    }
    addAt(g->board,next);
	addPiece(g->snake,next);
}

void serverAddSnake(Game *g, int id){
    g->snakes[id] = initSnake(up);
    Coordinate pos = getStartPosition(g->board);
    pos.ch = 48+id;
    g->snake = g->snakes[id];
    addAt(g->board, pos);
    addPiece(g->snake, pos);
    addAt(g->board, nextHead(g->snake));
    addPiece(g->snake, nextHead(g->snake));
    addAt(g->board, nextHead(g->snake));
    addPiece(g->snake, nextHead(g->snake));
    redraw(g);
}

void dfs(Snake *snake, Board *board, int prevy, int prevx, int cury, int curx, chtype ch){
    int nexty = -1, nextx = -1;
    if (!(cury+1==prevy && curx==prevx) && getAtYX(board, cury+1, curx)==ch) nexty = cury+1, nextx = curx; 
    if (!(cury-1==prevy && curx==prevx) && getAtYX(board, cury-1, curx)==ch) nexty = cury-1, nextx = curx; 
    if (!(cury==prevy && curx+1==prevx) && getAtYX(board, cury, curx+1)==ch) nexty = cury, nextx = curx+1; 
    if (!(cury==prevy && curx-1==prevx) && getAtYX(board, cury, curx-1)==ch) nexty = cury, nextx = curx-1; 
    if (nexty==-1 && nextx==-1) return;
    addPiece(snake, initCoordinate(nexty, nextx, ch));
    dfs(snake, board, cury, curx, nexty, nextx, ch);
}

void clientAddSnake(Game *g, GameInfo gi, bool isNewSnake){
    g->snakes[gi.uid] = initSnake(gi.dir);
    Coordinate pos = initCoordinate(gi.y, gi.x, 48+gi.uid);
    if (isNewSnake){
        addAt(g->board, pos);
        addPiece(g->snakes[gi.uid], pos);
        addAt(g->board, nextHead(g->snakes[gi.uid]));
        addPiece(g->snakes[gi.uid], nextHead(g->snakes[gi.uid]));
        addAt(g->board, nextHead(g->snakes[gi.uid]));
        addPiece(g->snakes[gi.uid], nextHead(g->snakes[gi.uid]));
        redraw(g);
    }else{
        addPiece(g->snakes[gi.uid], pos);
        dfs(g->snakes[gi.uid], g->board, -1, -1, gi.y, gi.x, 48+gi.uid);
    }
}

void removeSnake(Game *g, int id){
    while (size(g->snakes[id])>0){
        int empty_row = gety(tail(g->snakes[id]));
        int empty_col = getx(tail(g->snakes[id]));
        addAt(g->board,initCoordinate(empty_row,empty_col,' '));
        removePiece(g->snakes[id]);
    }
    free(g->snakes[id]);
    g->snakes[id] = NULL;
}

void setUserDir(Game *g, int id, Direction dir){
    setDirection(g->snakes[id], dir);
}

void startGame(Game *g){
    fprintf(stderr, "[Game] Start game\n");
	while (g->type==SERVER || !isOver(g)){
		processInput(g);
		updateState(g);
		redraw(g);
	}
    fprintf(stderr,"[Game] End game\n");
	// getch();
	endwin();
}

void showGameInfo(Game *g){
    char fmstr[105];
    sprintf(fmstr, "apple y: %d, apple x: %d, gameover: %d, type: %d, id: %d, fd: %d, %d %d %d %d %d %d %d%d%d%d%d%d\n", gety(*(g->apple)), getx(*(g->apple)), g->game_over, g->type, g->id, g->fd, g->snake->snake_piece[0].y, g->snake->snake_piece[0].x, g->snake->snake_piece[1].y, g->snake->snake_piece[1].x, g->snake->snake_piece[2].y, g->snake->snake_piece[2].x, g->snakes[0]==NULL, g->snakes[1]==NULL, g->snakes[2]==NULL, g->snakes[3]==NULL, g->snakes[4]==NULL, g->snakes[5]==NULL);
    addGameInfo(g->board, fmstr);
}

bool isOver(const Game *g){
	return g->game_over;
}
