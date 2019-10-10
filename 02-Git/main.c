#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// PRS enumeration
enum PRS
{
	PRS_PAPER = 0,
	PRS_ROCK = 1,
	PRS_SCISSORS = 2
};

// function prototypes
int getUserChoice();
int getCompChoise();
void calculateResult(int userChoice, int compChoise);

// main function
int main()
{
	int userChoise = getUserChoice();
	int compChoise = getCompChoise();
	calculateResult(userChoise, compChoise);

	//
	return 0;
}

// get user choice funtion
int getUserChoice()
{
	int choise;

	printf("Enter your choice (0-paper, 1-rock, 2-scissors) > ");
	fscanf(stdin, "%d", &choise);
		
	// result
	return choise;
}

// get computer choise function
int getCompChoise()
{
	srand(time(NULL)); 
	int choise = rand() % 3;
	switch(choise)
	{
		case PRS_PAPER: printf("Computer choose paper\n"); break;
		case PRS_ROCK: printf("Computer choose rock\n"); break;
		case PRS_SCISSORS: printf("Computer chose scissors\n"); break;
	}

	// result
	return choise;
}

// get result function
void calculateResult(int userChoice, int compChoise)
{
	if(userChoice == compChoise)
		printf("Equal\n");
	else if((userChoice == PRS_PAPER && compChoise == PRS_ROCK) || 
		(userChoice == PRS_ROCK && compChoise == PRS_SCISSORS) || 
		(userChoice == PRS_SCISSORS && compChoise == PRS_PAPER)) 
		printf("You win!!!\n");
	else
		printf("You lose\n");
}

