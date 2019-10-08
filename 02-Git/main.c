#include <stdio.h>

void Print_Help();
void RunGame();

int main(int argc, char *argv[]) {
    RunGame();
    return 0;
}

void Print_Help() {
    printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
}

void RunGame() {
    Print_Help();
}
