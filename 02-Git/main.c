#include "stdio.h"
#include "stdlib.h"
#include <time.h>
#include <math.h>
#include "main.h"

char* str[] = { "paper", "scissors", "rock" };
char c = 0;
int usr_choice = -1;

int GenRandomRPS(void)
{
    float rand_x;

    rand_x = (float)(rand()) / RAND_MAX;
    return rand_x * 3;
}

void GameProcess(int user, int pc)
{
    if (user < 0)
        return;

    printf("You choose %s, I choose %s\n", str[user], str[pc]);

    if (pc > user)
    {
        if ((pc == 2) && (user == 0))
        {
            printf("You win: %s beats %s\n", str[user], str[pc]);
            return;
        }
        printf("I win: %s beats %s\n", str[pc], str[user]);
        return;
    }

    if (pc == user)
        printf("Dead heat: no one lose or win");
    else
    {
        if ((pc == 0) && (user == 2))
        {
            printf("I win: %s beats %s\n", str[pc], str[user]);
            return;
        }
        printf("You win: %s beats %s\n", str[user], str[pc]);
    }

    return;
}

int main(int argc, char* argv[])
{
    srand(time(NULL));

    printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
    c = getchar();

    if (c == 'p')
        usr_choice = 0;
    if (c == 's')
        usr_choice = 1;
    if (c == 'r')
        usr_choice = 2;

    GameProcess(usr_choice, GenRandomRPS());

    while (getchar() != 'q');

    exit(0);
}
