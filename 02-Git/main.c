#include <stdio.h>
#include <time.h>
#include <stdlib.h>

char get_my_choice()
{
        return (char)(rand() % 3);
}

char get_user_choice()
{
        char input = 0;

        do {
                if (input) {
                        printf("Incorrect input!\n\n");
                }
                printf("Please choose: rock (r) - paper (p) - scissors (s) - quit (q)\n>>> ");
                scanf(" %c", &input);
        } while (input < 112 || input > 115);

        switch (input) {
        case 112:
                return 0;
        case 113:
                return 3;
        case 114:
                return 1;
        case 115:
                return 2;
        default:
                return 0;
        }
}

void print_result(char my_choice, char opponent_choice)
{
        const char *variants[] = {"paper", "rock", "scissors"};

        printf("You chose %s, I chose %s.\n", variants[(int)opponent_choice],
                variants[(int)my_choice]);

        if (my_choice == opponent_choice) {
                printf("Draw: we both chose %s.\n\n", variants[(int)my_choice]);
        } else {
                if ((my_choice == 0 && opponent_choice == 1) ||
                        (my_choice == 1 && opponent_choice == 2) ||
                        (my_choice == 2 && opponent_choice == 0))
                        printf("I win: %s beats %s.\n\n",
                                variants[(int)my_choice],
                                variants[(int)opponent_choice]);
                else
                        printf("You win: %s beats %s.\n\n",
                                variants[(int)opponent_choice],
                                variants[(int)my_choice]);
        }
}

int main()
{
        char user_choice;
        char my_choice;

        srand(time(NULL));

        while (1) {
                my_choice = get_my_choice();
                user_choice = get_user_choice();
                if (user_choice == 3)
                        break;
                print_result(my_choice, user_choice);
        }

        return 0;
}
