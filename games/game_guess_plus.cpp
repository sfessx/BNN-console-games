#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <string>
#include <conio.h>

typedef void (*GetStatsFunc)(const char*, char*);
typedef void (*UpdateExtFunc)(const char*, const char*, int, const char*);

extern "C" {
    __declspec(dllexport) const char* getGameName() {
        return "Guess Number Game Plus"; 
    }

    __declspec(dllexport) void playGame(const char* username) {
        std::system("cls");
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        
        DWORD dwMode = 0; GetConsoleMode(hConsole, &dwMode);
        SetConsoleMode(hConsole, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

        CONSOLE_CURSOR_INFO cursorInfo; GetConsoleCursorInfo(hConsole, &cursorInfo);
        cursorInfo.bVisible = TRUE; SetConsoleCursorInfo(hConsole, &cursorInfo);

        int current_high = 0;
        HMODULE hStats = LoadLibrary("libs\\stats.dll");
        if (hStats) {
            GetStatsFunc getStats = (GetStatsFunc)GetProcAddress(hStats, "getStats");
            if (getStats) {
                char buf[64] = "GET_GUESS_PLUS_HIGH";
                getStats(username, buf);
                current_high = std::atoi(buf);
            }
            FreeLibrary(hStats);
        }

        std::cout << "\033[1;95m=== Guess Number Plus (1-1000) ===\033[0m\n";
        std::cout << "User: " << username << " | Record: \033[93m" << current_high << "\033[0m pts\n";
        std::cout << "--------------------------------------------------------\n";
            
        std::srand(std::time(0));
        int target = std::rand() % 1000 + 1;
        int guess = 0;
        int attempts = 0;
        int maxScore = 1000;
        
        while (true) {
            std::cout << "\033[97mEnter your guess (1-1000) or 0 to exit:\033[0m ";
            if (!(std::cin >> guess)) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "\033[91mInvalid input! Only numbers allowed.\033[0m\n";
                continue;
            }
            if (guess == 0) {
                std::cin.ignore(10000, '\n');
                return;
            }
                    
            if (guess == target) {
                int finalScore = maxScore - (attempts * 80);
                if (finalScore < 40) finalScore = 40;
                
                std::cin.ignore(10000, '\n');
                cursorInfo.bVisible = FALSE; SetConsoleCursorInfo(hConsole, &cursorInfo);

                int totalAttempts = attempts + 1;

                std::cout << "\n\033[92;1mCORRECT! You conquered the 1-1000 challenge!\033[0m\n";
                std::cout << "Total Attempts: " << totalAttempts << " times\n";
                std::cout << "Your Score: \033[1;93m" << finalScore << "\033[0m points!\n";

                hStats = LoadLibrary("libs\\stats.dll");
                if (hStats) {
                    UpdateExtFunc updateScoreExtended = (UpdateExtFunc)GetProcAddress(hStats, "updateScoreExtended");
                    if (updateScoreExtended) {
                        updateScoreExtended(username, "GuessNumberPlus", finalScore, "pts");
                    }
                    FreeLibrary(hStats);
                }

                if (totalAttempts <= 10) {
                    std::cout << "\n\033[96;1mPERFECT CLEAR! You unlocked the legendary reward!\033[0m\n";
                    std::cout << "\033[97mPress Enter to return to menu...\033[0m\n";

                    int flashFlip = 0;
                    while (_kbhit()) _getch();

                    while (!_kbhit()) {
                        COORD winCoord = {0, 12}; 
                        SetConsoleCursorPosition(hConsole, winCoord);
                        
                        if (flashFlip % 2 == 0) {
                            std::cout << "\033[92;1m";
                            std::cout << "   ######   #######   ########   ########   ########   ######   ########  ##\n";
                            std::cout << "  ##    ## ##     ##  ##     ##  ##     ##  ##        ##    ##     ##     ##\n";
                            std::cout << "  ##       ##     ##  ########   ########   ######    ##           ##     ##\n";
                            std::cout << "  ##    ## ##     ##  ##   ##    ##   ##    ##        ##    ##     ##       \n";
                            std::cout << "   ######   #######   ##    ##   ##    ##   ########   ######      ##     ##\n\033[0m";
                        } else {
                            std::cout << "\033[93;1m";
                            std::cout << "   ######   #######   ########   ########   ########   ######   ########  ##\n";
                            std::cout << "  ##    ## ##     ##  ##     ##  ##     ##  ##        ##    ##     ##     ##\n";
                            std::cout << "  ##       ##     ##  ########   ########   ######    ##           ##     ##\n";
                            std::cout << "  ##    ## ##     ##  ##   ##    ##   ##    ##        ##    ##     ##       \n";
                            std::cout << "   ######   #######   ##    ##   ##    ##   ########   ######      ##     ##\n\033[0m";
                        }
                        flashFlip++;
                        Sleep(300);
                    }
                    _getch();
                } 
                else {
                    std::cout << "\n\033[90mClear confirmed. Challenge harder next time for the Secret Reward.\033[0m\n";
                    std::cout << "\nPress Enter to return to menu...";
                    while (_kbhit()) _getch();
                    std::cin.get();
                }
                break;
            }
            attempts++;
            
            if (guess > target) {
                std::cout << "-> \033[93mToo big! (Aim Lower)\033[0m\n";
            } else {
                std::cout << "-> \033[96mToo small! (Aim Higher)\033[0m\n";
            }
        }
        cursorInfo.bVisible = TRUE; SetConsoleCursorInfo(hConsole, &cursorInfo);
    }
}