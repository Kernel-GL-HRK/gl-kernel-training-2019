#include <stdio.h>
#include <stdbool.h>

#define NUM_STATE 3

typedef enum {
    G_ROCK,
    G_PAPER,
    G_SCISSORS,
    G_GARBAGE
} G_STATES;

typedef struct {
    G_STATES state;
    char *long_name;
    char short_name;
    char *descriptions;
} Options;

const Options parse_args[NUM_STATE] = {
    {G_ROCK, "rock", 'r', "Rock beats scissors"},
    {G_PAPER, "paper", 'p', "Paper beats rock"},
    {G_SCISSORS, "scissors", 's', "Scissors beats paper"},
};

Options *GetPlayerChoice();
void Print_Help();
void RunGame();

bool isGameRun = true;

int main(int argc, char *argv[]) {
    RunGame();
    return 0;
}

Options *ParseChoice(const char choice) {
    for (int s = 0; s < NUM_STATE; ++s) {
        if (choice == parse_args[s].short_name) {
            return (Options *)&parse_args[s];
        }
    }
    return NULL;
}

Options *GetPlayerChoice() {
    char player_choice;
    Options *ret;

    scanf(" %c", &player_choice);
    printf("\n");
    ret = ParseChoice(player_choice);

    for (int s = 0; s < NUM_STATE; ++s) {
        if (player_choice == parse_args[s].short_name) {
            ret = (Options *) & parse_args[s];
            break;
        }
    }

    if (ret == NULL) {
        Print_Help();
        isGameRun = false;
    }

    return ret;
}

void Print_Help() {
    printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
}

void RunGame() {
    Print_Help();
    isGameRun = true;
    while (isGameRun) {
        const Options *player = GetPlayerChoice();
    }
}
