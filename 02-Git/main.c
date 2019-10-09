#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {

    char human_val = 0, computer_val = 0;
    time_t t;
    const char str [3][10] = {"rock\n", "paper\n", "scissors\n"};
    const char symbols[3] = {'r', 'p', 's'};

    printf("rock-r, paper-p, scissors-s. Make your choose: \n");

    // read value from keyboard
    while ((human_val != 'r')
            && (human_val != 'p')
            && (human_val != 's')) {
        human_val = getchar();
    }

    // generate random value for computer
    srand((unsigned) time(&t));
    int index = rand() % 3;
    computer_val = symbols[index];

    printf("Computer: %s", str[index]);

    // comaring values and detection of winer
    if (computer_val == human_val) {
        printf("Nobody wins\n");
    }
    else if (((computer_val == 'r') && (human_val == 'p')) ||
             ((computer_val == 'p') && (human_val == 's')) ||
             ((computer_val == 's') && (human_val == 'r'))) {
        printf("Human wins\n");
    }
    else {
        printf("Computer wins\n");
    }

    return 0;

}
