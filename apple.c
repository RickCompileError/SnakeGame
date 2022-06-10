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
//         fprintf(stderr,"Apple generate at Y: %d X: %d\n",gety(*g->apple),getx(*g->apple));
     }
}

void deleteApple(Game *g){
    free(g->apple);
    g->apple = NULL;
}
