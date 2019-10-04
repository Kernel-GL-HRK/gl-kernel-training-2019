#include <stdio.h>
#include <time.h>
#include <stdlib.h>

char get_my_choise()
{
        return (char)(rand() % 3);
}

char get_user_choise()
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

void print_result(char my_choise, char opponent_choise)
{
        const char *variants[] = {"paper", "rock", "scissors"};

        printf("You chose %s, I chose %s.\n", variants[opponent_choise],
                variants[my_choise]);

        if (my_choise == opponent_choise) {
                printf("Draw: we both chose %s.\n\n", variants[my_choise]);
        } else {
                if ((my_choise == 0 && opponent_choise == 1) ||
                        (my_choise == 1 && opponent_choise == 2) ||
                        (my_choise == 2 && opponent_choise == 0))
                        printf("I win: %s beats %s.\n\n",
                                variants[my_choise],
                                variants[opponent_choise]);
                else
                        printf("You win: %s beats %s.\n\n",
                                variants[opponent_choise],
                                variants[my_choise]);
        }
}

int main()
{
        char user_choise;
        char my_choise;

        srand(time(NULL));

        while (1) {
                my_choise = get_my_choise();
                user_choise = get_user_choise();
                if (user_choise == 3)
                        break;
                print_result(my_choise, user_choise);
        }

        return 0;
}
