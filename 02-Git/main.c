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

// main function
int main()
{
	int userChoise = getUserChoice();
	int compChoise = getCompChoise();

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

