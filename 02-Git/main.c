#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define DRAW 0
#define WIN 1
#define LOSE 2

int main(int argc, char * argv[])
{
	char *userChoose, *gameChoose;
	int gameState = DRAW;
	
	printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
	/* User */
	switch (getchar()) {
	case 'r':
		userChoose = "rock";
		break;
	case 'p':
		userChoose = "paper";
		break;
	case 's':
		userChoose = "scissors";
		break;
	default :
		printf("Not correct. Exit\n");
		return 0;
	}
	/* Game */
	srand(time(NULL));
	switch (rand() % 3) {
	case 0:
		gameChoose = "rock";
		if (userChoose == gameChoose) {
			gameState = DRAW;
		} else if (strcmp (userChoose, "scissors")==0) {
			gameState = WIN;
		} else {
			gameState = LOSE;
		}
		break;
	case 1:
		gameChoose = "paper";
		if (userChoose == gameChoose) {
			gameState = DRAW;
		} else if (strcmp (userChoose, "rock")==0) {
			gameState = WIN;
		} else {
			gameState = LOSE;
		}
		break;
	case 2:
		gameChoose = "scissors";
		if (userChoose == gameChoose) {
			gameState = DRAW;
		} else if (strcmp (userChoose, "paper")==0) {
			gameState = WIN;
		} else {
			gameState = LOSE;
		}
		break;
	}
	printf("You choose %s, I choose %s\n",userChoose, gameChoose);
	if (gameState == WIN) {
		printf("I win: %s beats %s\n",gameChoose, userChoose);
	} else if (gameState == LOSE) {
		printf("I lose: %s beats %s\n",userChoose, gameChoose);
	} else {
		printf("DRAW: %s = %s\n",userChoose, gameChoose);
	}
	return 0;
}
