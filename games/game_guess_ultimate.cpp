#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <string>
#include <conio.h>

typedef void (*UpdateExtFunc)(const char*, const char*, int, const char*);
typedef void (*GetStatsFunc)(const char*, char*);

void RewardAnimation()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int flash = 0;
    while (!_kbhit())
    {
        system("cls");
        if (flash % 2 == 0)
            SetConsoleTextAttribute(hConsole, 12);
        else
            SetConsoleTextAttribute(hConsole, 14);
        std::cout <<
        "\n"
        " ####   ####  #####  #####  #####  ######\n"
        "##    ##  ## ##     ##     ##        ##\n"
        "## ## #####  ####   ####   ##        ##\n"
        "##  ## ##  ## ##     ##     ##       ##\n"
        " ####  ##  ## #####  #####  #####    ##\n";
        SetConsoleTextAttribute(hConsole, 15);
        std::cout
        << "\n\n"
        << "GODLIKE SPEED!\n"
        << "You unlocked the legendary perfection!\n";
        flash++;
        Sleep(300);
    }
    while (_kbhit())
        _getch();
    SetConsoleTextAttribute(hConsole, 7);
}

extern "C"
{
__declspec(dllexport)
const char* getGameName()
{
    return "Guess Number Ultimate";
}

__declspec(dllexport)
void playGame(const char* username)
{
    system("cls");
    int record = 0;
    HMODULE stats = LoadLibraryA("libs\\stats.dll");
    if(stats)
    {
        GetStatsFunc getStats =
        (GetStatsFunc)GetProcAddress(
            stats,
            "getStats"
        );
        if(getStats)
        {
            char buffer[64]="GET_GUESS_ULTIMATE_HIGH";
            getStats(
                username,
                buffer
            );
            record = atoi(buffer);
        }
        FreeLibrary(stats);
    }
    std::cout
    << "=== Guess Number Ultimate (1-10000) ===\n";
    std::cout
    << "User: "
    << username
    << " | Record: "
    << record
    << " pts\n";
    std::cout
    << "--------------------------------------\n";
    srand((unsigned)time(0));
    int target = rand()%10000+1;
    int guess;
    int attempts=0;
    while(true)
    {
        std::cout
        << "\nEnter your guess (1-10000) or 0 to exit: ";
        if(!(std::cin>>guess))
        {
            std::cin.clear();
            std::cin.ignore(10000,'\n');
            std::cout
            << "Invalid input!\n";
            continue;
        }
        if(guess==0)
            return;
        attempts++;
        if(guess==target)
        {
            int score =
            10000 - (attempts-1)*800;
            if(score<400)
                score=400;
            std::cout
            << "\nCORRECT! You conquered the ultimate 1-10000 challenge!\n";
            std::cout
            << "Total Attempts: "
            << attempts
            << "\n";
            std::cout
            << "Your Score: "
            << score
            << " points!\n";
            stats=LoadLibraryA("libs\\stats.dll");
            if(stats)
            {
                UpdateExtFunc update =
                (UpdateExtFunc)GetProcAddress(
                    stats,
                    "updateScoreExtended"
                );
                if(update)
                {
                    update(
                        username,
                        "GuessNumberUltimate",
                        score,
                        "pts"
                    );
                }
                FreeLibrary(stats);
            }
            if(attempts<=15)
            {
                RewardAnimation();
            }
            break;
        }
        if(guess>target)
        {
            std::cout
            << "Too big! Aim Lower.\n";
        }
        else
        {
            std::cout
            << "Too small! Aim Higher.\n";
        }
    }
    std::cout
    << "\nPress Enter to return...";
    std::cin.ignore(10000,'\n');
    std::cin.get();
}
}