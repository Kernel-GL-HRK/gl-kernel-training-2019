#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "string.h"
int main(){
    printf("Welcome to the console game 'rock-paper-scissors'\n");
    char symbol[10];
    srand(time(NULL));
    const char roll[] = {'r','p','s'};
    char choose_pc[10];
    unsigned int stat[2];
    while(1){
        printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
        scanf("%s",symbol);
        choose_pc[0] = roll[(rand()%3)];
        switch (symbol[0]){
            case 'r':
                printf("You choose 'r', I choose %s \n",(char*)&choose_pc);
                if('p' == choose_pc[0])
                {
                    printf("===========You lose===========\n");
                    stat[0]++;
                }
                else
                {
                    printf("===========You win===========\n");
                    stat[1]++;
                }
                break;
            case 'p':
                printf("You choose 'p', I choose %s \n",(char*)&choose_pc);
                if('s' == choose_pc[0])
                {
                    printf("===========You lose===========\n");
                    stat[0]++;
                }
                else
                {
                    printf("===========You win===========\n");
                    stat[1]++;
                }
                break;
            case 's':
                printf("You choose 's', I choose %s \n",(char*)&choose_pc);
                if('r' == choose_pc[0])
                {
                    printf("===========You lose===========\n");
                    stat[0]++;
                }
                else
                {
                    printf("===========You win===========\n");
                    stat[1]++;
                }
                break;
            default:
                printf("Error\n");
                continue;
        }
        printf("Statistic: You win: %d --- You lose %d\n",stat[1],stat[0]);
        printf(" \n New Game \n");
    }
}
