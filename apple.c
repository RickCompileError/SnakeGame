#include "snakeio.h"

void createApple(Game *g){
     if (g->apple == NULL){
         int y, x;
         getEmptyCoordinates(g->board,&y,&x);
         g->apple = malloc(sizeof(Coordinate));
         g->apple->y = y;
         g->apple->x = x;
         g->apple->ch = 'A';
         addAt(g->board,*g->apple);
         fprintf(stderr,"[Apple] Apple generate at Y: %d X: %d\n",gety(*g->apple),getx(*g->apple));
     }
}

void addApple(Game *g, Coordinate new_apple){
    if (g->apple == NULL){
        g->apple = &new_apple;
        addAt(g->board, new_apple);
    }
}

void deleteApple(Game *g){
    free(g->apple);
    g->apple = NULL;
}

int getAppleY(Coordinate *apple){
    return gety(*apple);
}

int getAppleX(Coordinate *apple){
    return getx(*apple);
}
