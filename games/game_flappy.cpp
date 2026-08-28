#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <string>

typedef void (*GetStatsFunc)(const char*, char*);
typedef void (*UpdateExtFunc)(const char*, const char*, int, const char*);

const int SCREEN_WIDTH = 20;
const int SCREEN_HEIGHT = 14;

extern "C" {
    __declspec(dllexport) const char* getGameName() {
        return "Flappy Bird";
    }

    __declspec(dllexport) void playGame(const char* username) {
        std::system("cls");
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        
        DWORD dwMode = 0; GetConsoleMode(hConsole, &dwMode);
        SetConsoleMode(hConsole, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

        CONSOLE_CURSOR_INFO cursorInfo; GetConsoleCursorInfo(hConsole, &cursorInfo);
        cursorInfo.bVisible = FALSE; SetConsoleCursorInfo(hConsole, &cursorInfo);

        int current_high = 0;
        HMODULE hStats = LoadLibrary("libs\\stats.dll");
        if (hStats) {
            GetStatsFunc getStats = (GetStatsFunc)GetProcAddress(hStats, "getStats");
            if (getStats) {
                char buf[64] = "GET_FLAPPY_HIGH";
                getStats(username, buf);
                current_high = std::atoi(buf);
            }
            FreeLibrary(hStats);
        }

        std::srand(std::time(0));
        
        double birdY = SCREEN_HEIGHT / 2.0;
        double velocity = 0.0;
        const double GRAVITY = 0.12;
        const double JUMP_STRENGTH = -0.75;

        int pipeX = SCREEN_WIDTH - 4;
        int gapY = std::rand() % (SCREEN_HEIGHT - 6) + 1;
        const int GAP_SIZE = 4;

        int score = 0;
        bool gameOver = false;
        bool wLock = false;
        int frameCounter = 0;

        while(_kbhit()) _getch();

        while (!gameOver) {
            if ((GetAsyncKeyState('W') & 0x8000) || (GetAsyncKeyState(VK_UP) & 0x8000)) {
                if (!wLock) {
                    velocity = JUMP_STRENGTH;
                    wLock = true;
                }
            } else { wLock = false; }

            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;

            velocity += GRAVITY;
            birdY += velocity;

            if (birdY <= 0 || birdY >= SCREEN_HEIGHT) { gameOver = true; break; }

            frameCounter++;
            if (frameCounter % 3 == 0) {
                pipeX--;
            }

            if (pipeX < 1) {
                pipeX = SCREEN_WIDTH - 3;
                gapY = std::rand() % (SCREEN_HEIGHT - 6) + 1;
                score += 10;
            }

            int currentBirdY = static_cast<int>(birdY);
            if (pipeX == 3) {
                if (currentBirdY < gapY || currentBirdY >= gapY + GAP_SIZE) {
                    gameOver = true;
                    break;
                }
            }

            COORD coord = {0, 0}; SetConsoleCursorPosition(hConsole, coord);
            std::string out = "\033[0m=== Flappy Bird ===\nUser: " + std::string(username) + " | \033[93mScore: " + std::to_string(score) + "\033[0m | Record: " + std::to_string(current_high) + "\n";
            
            for (int y = 0; y < SCREEN_HEIGHT; y++) {
                out += "##";
                for (int x = 0; x < SCREEN_WIDTH; x++) {
                    if (x == 3 && y == currentBirdY) {
                        out += "\033[93;1m><\033[0m"; 
                    }
                    else if (x == pipeX) {
                        if (y < gapY || y >= gapY + GAP_SIZE) {
                            out += "\033[92m[]\033[0m"; 
                        } else {
                            out += "  ";
                        }
                    }
                    else {
                        out += "  ";
                    }
                }
                out += "##\n";
            }
            out += "================================================\n";
            out += "Controls: [W / UP] Flap  |  [ESC] Exit\n";
            std::cout << out;

            Sleep(80);
        }

        std::cout << "\n\033[91mGAME OVER!\033[0m Your Score: " << score << " points.\n";
        hStats = LoadLibrary("libs\\stats.dll");
        if (hStats) {
            UpdateExtFunc updateScoreExtended = (UpdateExtFunc)GetProcAddress(hStats, "updateScoreExtended");
            if (updateScoreExtended) {
                updateScoreExtended(username, "FlappyBird", score, "pts"); 
            }
            FreeLibrary(hStats);
        }
        
        cursorInfo.bVisible = TRUE; SetConsoleCursorInfo(hConsole, &cursorInfo);
        std::cout << "Press Enter to return to menu...";
        while (_kbhit()) _getch();
        std::cin.get();
    }
}