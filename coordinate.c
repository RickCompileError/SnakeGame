#include "snakeio.h"

Coordinate initCoordinate(int y, int x, chtype ch){
	Coordinate c = {.y = y, .x = x, .ch = ch};
	return c;
}

int gety(Coordinate c){
	return c.y;
}

int getx(Coordinate c){
	return c.x;
}

chtype getchtype(Coordinate c){
	return c.ch;
}
