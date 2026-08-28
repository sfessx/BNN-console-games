#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <string>
#include <cmath> 

typedef void (*GetStatsFunc)(const char*, char*);
typedef void (*UpdateExtFunc)(const char*, const char*, int, const char*);

const int S_WIDTH = 20;
const int S_HEIGHT = 15;
const int MAX_ALIENS_ON_SCREEN = 5; 

struct Laser { int x, y; bool active; };
struct Alien { 
    double realX;
    double realY;
    int x;
    int y;
    bool alive; 
    double speedY;
    double waveSpeed;
    double waveAmp;
    double centerX;
    int seedOffset;
};

Alien s_generateSingleAlien() {
    Alien a;
    a.realY = 0.0; 
    a.y = 0;
    a.alive = true;
    a.speedY = ((std::rand() % 6) + 4) / 100.0;
    a.waveSpeed = ((std::rand() % 10) + 5) / 100.0;
    a.waveAmp = ((std::rand() % 12) + 8) / 10.0;
    a.centerX = (std::rand() % (S_WIDTH - 6)) + 3;
    a.realX = a.centerX;
    a.x = static_cast<int>(a.realX);
    a.seedOffset = std::rand() % 1000;
    return a;
}

extern "C" {
    __declspec(dllexport) const char* getGameName() {
        return "Space Invaders"; 
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
                char buf[64] = "GET_SPACE_HIGH"; 
                getStats(username, buf);
                current_high = std::atoi(buf);
            }
            FreeLibrary(hStats);
        }

        std::srand(std::time(0));
        
        int playerX = S_WIDTH / 2;
        std::vector<Laser> lasers;
        std::vector<Alien> aliens;
        
        for (int i = 0; i < MAX_ALIENS_ON_SCREEN; i++) {
            Alien a = s_generateSingleAlien();
            a.realY = - (i * 3.0); 
            a.y = static_cast<int>(a.realY);
            aliens.push_back(a);
        }

        int score = 0;
        bool gameOver = false;
        bool spaceLock = false; 

        while(_kbhit()) _getch();

        while (!gameOver) {
            if (GetAsyncKeyState('A') & 0x8000) { if (playerX > 0) playerX--; }
            if (GetAsyncKeyState('D') & 0x8000) { if (playerX < S_WIDTH - 1) playerX++; }
            
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                if (!spaceLock) {
                    lasers.push_back({ playerX, S_HEIGHT - 2, true });
                    spaceLock = true;
                }
            } else { spaceLock = false; }

            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;

            for (auto& laser : lasers) {
                if (laser.active) {
                    laser.y--;
                    if (laser.y < 0) laser.active = false;
                }
            }

            for (auto& alien : aliens) {
                alien.realY += alien.speedY;
                alien.y = static_cast<int>(alien.realY);

                if (alien.realY > 0) {
                    double theta = (alien.realY * alien.waveSpeed) + alien.seedOffset;
                    alien.realX = alien.centerX + (std::sin(theta) * alien.waveAmp);
                    alien.x = static_cast<int>(alien.realX);
                    
                    if (alien.x < 0) alien.x = 0;
                    if (alien.x >= S_WIDTH) alien.x = S_WIDTH - 1;
                }
                if (alien.y >= S_HEIGHT) {
                    alien = s_generateSingleAlien();
                }
            }

            for (auto& laser : lasers) {
                if (!laser.active) continue;
                for (auto& alien : aliens) {
                    if (alien.realY > 0 && laser.y == alien.y && laser.x == alien.x) {
                        laser.active = false;
                        score += 30;
                        alien = s_generateSingleAlien();
                        break;
                    }
                }
            }

            for (const auto& alien : aliens) {
                if (alien.y == S_HEIGHT - 1 && alien.x == playerX) {
                    gameOver = true;
                    break;
                }
            }
            if (gameOver) break;

            COORD coord = {0, 0}; SetConsoleCursorPosition(hConsole, coord);
            std::string out = "\033[0m=== Space Invaders ===\n";
            out += "User: " + std::string(username) + " | \033[93mScore: " + std::to_string(score) + "\033[0m | Record: " + std::to_string(current_high) + " pts\n";
            out += "-----------------------------------------\n";
            
            for (int y = 0; y < S_HEIGHT; y++) {
                out += "##";
                for (int x = 0; x < S_WIDTH; x++) {
                    
                    bool hasLaser = false;
                    for (const auto& l : lasers) {
                        if (l.active && l.x == x && l.y == y) { hasLaser = true; break; }
                    }
                    
                    bool hasAlien = false;
                    for (const auto& alien : aliens) {
                        if (alien.realY > 0 && alien.x == x && alien.y == y) { hasAlien = true; break; }
                    }

                    if (hasLaser) {
                        out += "\033[91m| \033[0m";
                    }
                    else if (y == S_HEIGHT - 1 && x == playerX) {
                        out += "\033[92;1mA \033[0m";
                    }
                    else if (hasAlien) {
                        out += "\033[93;1mXX\033[0m";
                    }
                    else {
                        out += "  ";
                    }
                }
                out += "##\n";
            }
            out += "=========================================\n";
            out += "Controls: [A/D] Move | [SPACE] Fire Laser | [ESC] Exit\n";
            std::cout << out;

            Sleep(50); 
        }

        std::system("cls");
        std::cout << "\n\033[91mSHIP DESTROYED! Game Over!\033[0m\n";
        std::cout << "Your Final Score: \033[1;93m" << score << "\033[0m points.\n";

        hStats = LoadLibrary("libs\\stats.dll");
        if (hStats) {
            UpdateExtFunc updateScoreExtended = (UpdateExtFunc)GetProcAddress(hStats, "updateScoreExtended");
            if (updateScoreExtended) {
                updateScoreExtended(username, "SpaceInvaders", score, "pts"); 
            }
            FreeLibrary(hStats);
        }

        cursorInfo.bVisible = TRUE; SetConsoleCursorInfo(hConsole, &cursorInfo);
        std::cout << "\nPress Enter to return to menu...";
        while (_kbhit()) _getch();
        std::cin.get();
    }
}