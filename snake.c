#include "snakeio.h"

Snake* initSnake(Direction dir){
	Snake *s = malloc(sizeof(Snake));
	s->direction = dir;
	s->front = -1;
	s->rear = -1;
	s->sz = 0;
	s->snake_piece = malloc(QUEUE_SIZE*sizeof(Coordinate));
	return s;
}

void addPiece(Snake *s, Coordinate c){
	s->front = (s->front+1)%QUEUE_SIZE;
	if (s->front+1==s->rear){
		fprintf(stderr,"Queue is full!!!\n");
		exit(-1);
	}
	s->snake_piece[s->front] = c;
	s->sz++;
	if (s->rear==-1) s->rear = 0;
}

void removePiece(Snake *s){
	if (s->rear==s->front){
		fprintf(stderr,"Queue can't empty!!!\n");
		exit(-1);
	}
	s->rear = (s->rear+1)%QUEUE_SIZE;
	s->sz--;
}

Direction getDirection(Snake *s){
	return s->direction;
}

void setDirection(Snake *s, Direction dir){
    if (s->direction+dir!=0) s->direction = dir;
}

Coordinate head(Snake *s){
	return s->snake_piece[s->front];
}

Coordinate tail(Snake *s){
	return s->snake_piece[s->rear];
}

Coordinate nextHead(Snake *s){
	Coordinate cur_head = head(s);
	int y = gety(cur_head);
	int x = getx(cur_head);
	Direction cur_dir = getDirection(s);
	switch (cur_dir){
		case down:
			y++; break;
		case up:
			y--; break;
		case left:
			x--; break;
		case right:
			x++; break;
	}
	return initCoordinate(y,x,getchtype(cur_head));
}

int size(Snake *s){
	return s->sz;
}
