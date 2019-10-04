#include <iostream>
#include <ctime>
using namespace std;

enum
{
    Rock = 0, Paper, Scisors,
};

char pick[3] =
{
    'r',
    'p',
    's' };

int main(int argc, char **argv)
{
    char symb = ' ';
    int userPick = -1;

    cout << "Please choose: rock (r) - paper (p) - scissors (s)" << endl;
    cin >> symb;

    cout << "You choose " << pick[userPick] << endl;

    srand(time(0));
    int pcPick = random() % 3;
    cout << "PC choose " << pick[pcPick] << endl;

    return 0;
}

