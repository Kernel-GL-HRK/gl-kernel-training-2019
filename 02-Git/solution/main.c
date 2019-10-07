#include <stdio.h>
#include <stdlib.h>
#include <time.h>


enum elements_of_game {
	ROCK,
	SCISSORS,
	PAPER,
	INCOR_VAL
};


const enum elements_of_game mask_winner[3] = {PAPER, ROCK, SCISSORS};


const char* element_to_str(enum elements_of_game e_o_g)
{
	const char* const array_elements[] = {"rock", "scissors", "paper"};
	return array_elements[e_o_g];
}

char wait_and_get_user_input(void)
{
	char user_selected;
	printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
	scanf("%c", &user_selected);
	return user_selected;
}


enum elements_of_game char_convert_to_element(char c_t_e)
{
	switch (c_t_e) {
	case 'r':
		return ROCK;
	case 's':
		return SCISSORS;
	case 'p':
		return PAPER;
	default:
		return INCOR_VAL;
	}
}

enum elements_of_game generate_random_element(void)
{
	srand((unsigned int) time(NULL));
	const int upper_bound = INCOR_VAL;
	return (rand() % upper_bound);
}



void determinat_winner(enum elements_of_game pc, enum elements_of_game man)
{
	if (pc == man)
		printf("Draw\n");
	else if (mask_winner[pc] == man)
		printf("Man win\n");
	else
		printf("Computer win\n");
}



int main(void)
{

	char element = wait_and_get_user_input();
	enum elements_of_game user_selected =  char_convert_to_element(element);
	if (user_selected == INCOR_VAL)
		printf("Invalid user input\n");
	else
		printf("Man choose %s\n", element_to_str(user_selected));

		enum elements_of_game pc_choice = generate_random_element();
		printf("Computer chose %s\n", element_to_str(pc_choice));
		determinat_winner(pc_choice, user_selected);
	return 0;

}

