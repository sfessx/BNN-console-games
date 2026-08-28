#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>

typedef void (*GetStatsFunc)(const char*, char*);
typedef void (*UpdateExtFunc)(const char*, const char*, int, const char*);

extern "C" {
    __declspec(dllexport) const char* getGameName() {
        return "Guess Number Game";
    }

    __declspec(dllexport) void playGame(const char* username) {
        std::system("cls");
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        
        DWORD dwMode = 0; 
        GetConsoleMode(hConsole, &dwMode);
        SetConsoleMode(hConsole, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

        std::cout << "\033[1;95m=== Guess the Number ===\033[0m\n";
            
        int current_high = 0;
        HMODULE hStats = LoadLibrary("libs\\stats.dll");
        if (hStats) {
            GetStatsFunc getStats = (GetStatsFunc)GetProcAddress(hStats, "getStats");
            if (getStats) {
                char buf[64] = "GET_GUESS_HIGH";
                getStats(username, buf);
                current_high = std::atoi(buf);
            }
            FreeLibrary(hStats);
        }
        std::cout << "Your Guess High Score: \033[93m" << current_high << "\033[0m points\n";
        std::cout << "------------------------------------\n";
            
        std::srand(std::time(0));
        int target = std::rand() % 100 + 1;
        int guess = 0;
        int attempts = 0;
        
        while (true) {
            std::cout << "\033[97mGuess (1-100) or 0 to exit:\033[0m ";
            if (!(std::cin >> guess)) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "\033[91mInvalid input!\033[0m\n";
                continue;
            }
            if (guess == 0) return;
                    
            if (guess == target) {
                int score = 100 - (attempts * 10);
                if (score < 10) score = 10;
                            
                std::cout << "\n\033[92;1mCorrect! You used " << attempts + 1 << " attempts.\033[0m\n";
                std::cout << "Your Score: \033[1;93m" << score << "\033[0m points!\n";
                
                hStats = LoadLibrary("libs\\stats.dll");
                if (hStats) {
                    UpdateExtFunc updateScoreExtended = (UpdateExtFunc)GetProcAddress(hStats, "updateScoreExtended");
                    if (updateScoreExtended) {
                        updateScoreExtended(username, "GuessGame", score, "pts");
                    }
                    FreeLibrary(hStats);
                }
                break;
            }
            attempts++;
            
            if (guess > target) {
                std::cout << "\033[93mToo big!\033[0m\n";
            } else {
                std::cout << "\033[96mToo small!\033[0m\n";
            }
        }
        std::cin.ignore(10000, '\n');
        std::cout << "\nPress Enter to return...";
        std::cin.get();
    }
}