#include <stdio.h>
#include <stdlib.h>
#include <time.h>


enum elements_of_game {
	ROCK,
	SCISSORS,
	PAPER,
	INCOR_VAL
};


const char* element_to_str(enum elements_of_game e_o_g)
{
	const char* const array_elements[] = {"rock", "scissors", "paper"};
	return array_elements[e_o_g];
}

char wait_and_get_user_input(void)
{
	char user_select;
	printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
	scanf("%c", &user_select);
	return user_select;
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



int main(void)
{

	char element = wait_and_get_user_input();
	enum elements_of_game user_select =  char_convert_to_element(element);
	if (user_select == INCOR_VAL)
		printf("Invalid user input\n");
	else
		printf("Man choose %s\n", element_to_str(user_select));

		enum elements_of_game pc_choice = generate_random_element();

		printf("Computer chose %s\n", element_to_str(pc_choice));
	return 0;

}

