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

    bool symbOk = false;
    for (int i = 0; i < 3; i++)
    {
        if (pick[i] == symb)
        {
            symbOk = true;
            userPick = i;
            break;
        }
    }

    if (symbOk == false)
    {
        return 1;
    }

    cout << "You choose " << pick[userPick] << endl;

    srand(time(0));
    int pcPick = random() % 3;
    cout << "PC choose " << pick[pcPick] << endl;

    if (userPick == pcPick)
    {

    }
    else if (((userPick + 1) % 3) == pcPick)
    {
        /**pc win*/
        cout << "PC win" << endl;
    }
    else
    {
        /**user win*/
        cout << "You win" << endl;
    }

    return 0;
}

