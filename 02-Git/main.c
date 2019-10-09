#include <stdio.h>

// PRS enumeration
enum PRS
{
	PRS_PAPER = 0,
	PRS_ROCK = 1,
	PRS_SCISSORS = 2
};

// function prototypes
int getUserChoice();

// main function
int main()
{
	int userChoise = getUserChoice();

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

