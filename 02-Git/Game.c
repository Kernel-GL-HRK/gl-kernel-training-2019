#include <iostream>
#include <string>
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <locale>
#include <stdlib.h>

using namespace std;

/* Функция преобразует буквы в полное написание */
string rewrite(int c)
{
    if (c == 0) return "Камень";
    else if (c == 1) return "Ножницы";
    else return "Бумага";
}

int main()
{
    setlocale(LC_ALL, "Russian");

    srand(static_cast<unsigned>(time(NULL)));

    int user_wins = 0;
    int comp_wins = 0;

    int cc = rand() % 3;

    while ((user_wins < 10) && (comp_wins < 10))
    {
        cout << "Камень, ножницы, или бумага?(К/Н/Б): ";
        unsigned char cuser;
        cin >> cuser;
        int cu;
        switch (cuser)
        {
        case 138:
            cu = 0;break;
        case 141:
            cu = 1;break;
        case 129:
            cu = 2;break;
        default:
            cout << "Ошибка" << endl;
            continue;
        }

        int cc = rand() % 3;

        int answer = (cu - cc + 3) % 3;

        if (answer == 1) {
            comp_wins++;
            cout << rewrite(cu) << " против " << rewrite(cc) << ", Компьютер победил! (" << user_wins << ":" << comp_wins << ")" << endl;
        }
        else if (answer == 0) {
            cout << rewrite(cu) << " против " << rewrite(cc) << ", Ничья! (" << user_wins << ":" << comp_wins << ")" << endl;
        }
        else if (answer == 2) {
            user_wins++;
            cout << rewrite(cu) << " против " << rewrite(cc) << ", Вы победили! (" << user_wins << ":" << comp_wins << ")" << endl;
        }
    }

    if (comp_wins > user_wins) cout << endl << "Компьютер победил!";
    else if (comp_wins < user_wins) cout << endl << "Поздравляем! Вы победили";
    else cout << endl << "Ничья!";

    cout << endl << "Нажмите любую клавишу...";
    _getch();
    return 0;
}
