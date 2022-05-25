#include "snakeio.h"

int main(){
	fptr = fopen("log.txt","w");
	if (fptr == NULL){
		printf("ERROR to open log.txt file.");
		exit(-1);
	}
	fprintf(fptr,"Initialize FILE fptr.\n");
	Game *game = initGame();
	startGame(game);
}
