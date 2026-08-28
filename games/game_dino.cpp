#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <string>

typedef void (*GetStatsFunc)(const char*, char*);
typedef void (*UpdateExtFunc)(const char*, const char*, int, const char*);

const int SCREEN_WIDTH = 55; 
const int GROUND_Y = 11;
const int CEILING_Y = 4;

extern "C" {
    __declspec(dllexport) const char* getGameName() {
        return "Chrome Dino"; 
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
                char buf[64] = "GET_DINO_HIGH"; 
                getStats(username, buf);
                current_high = std::atoi(buf);
            }
            FreeLibrary(hStats);
        }

        std::srand(std::time(0));
        
        double dinoY = GROUND_Y;
        double velocity = 0.0;
        const double BASE_GRAVITY = 0.45;
        const double HOLD_GRAVITY = 0.16;
        const double FAST_FALL_GRAVITY = 2.4; 
        const double JUMP_START = -2.1;
        bool isJumping = false;

        int obsX = SCREEN_WIDTH - 5;
        int obsType = 0;
        int cacWidth = 1;  
        int cacHeight = 1; 

        int score = 0;
        bool gameOver = false;
        int baseSleep = 45; 
        int frameCount = 0; 

        ULONGLONG lastPhysicsTime = GetTickCount64();
        const ULONGLONG PHYSICS_INTERVAL = 40;

        while(_kbhit()) _getch();

        while (!gameOver) {
            bool isHoldingJump = (GetAsyncKeyState('W') & 0x8000) || (GetAsyncKeyState(VK_UP) & 0x8000) || (GetAsyncKeyState(VK_SPACE) & 0x8000);
            bool isHoldingDuck = (GetAsyncKeyState('S') & 0x8000) || (GetAsyncKeyState(VK_DOWN) & 0x8000);

            if (isHoldingJump && !isJumping && !isHoldingDuck) {
                velocity = JUMP_START;
                isJumping = true;
                lastPhysicsTime = GetTickCount64();
            }

            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;

            ULONGLONG currentTime = GetTickCount64();
            if (currentTime - lastPhysicsTime >= PHYSICS_INTERVAL) {
                if (isJumping) {
                    double currentGravity = BASE_GRAVITY;
                    if (isHoldingDuck) currentGravity = FAST_FALL_GRAVITY;
                    else if (isHoldingJump && velocity < 0) currentGravity = HOLD_GRAVITY;
                    
                    velocity += currentGravity;
                    dinoY += velocity;

                    if (dinoY <= CEILING_Y) {
                        dinoY = CEILING_Y;
                        if (velocity < 0) velocity = 0.0;
                    }
                    if (dinoY >= GROUND_Y) {
                        dinoY = GROUND_Y;
                        velocity = 0.0;
                        isJumping = false;
                    }
                }
                lastPhysicsTime = currentTime;
            }

            baseSleep = 45 - (score / 20);
            if (baseSleep < 16) baseSleep = 16;

            obsX--;

            if (obsX < 2) {
                obsX = SCREEN_WIDTH - 5 + (std::rand() % 16); 
                score += 10;

                if (score >= 100 && (std::rand() % 100 < 40)) {
                    obsType = (std::rand() % 2 == 0) ? 1 : 2; 
                } else {
                    obsType = 0; 
                    cacWidth = (std::rand() % 2 == 0) ? 2 : 1; 
                    cacHeight = (std::rand() % 2 == 0) ? 2 : 1; 
                }
            }

            int cDinoY = static_cast<int>(dinoY);
            int dinoBottomY = cDinoY; 
            int dinoTopY = (isHoldingDuck && !isJumping) ? cDinoY : (cDinoY - 1); 

            if (obsX == 7) { 
                if (obsType == 0) { 
                    int cacTopY = GROUND_Y - cacHeight + 1;
                    for (int y = dinoTopY; y <= dinoBottomY; y++) {
                        if (y >= cacTopY && y <= GROUND_Y) { gameOver = true; break; }
                    }
                }
                else if (obsType == 1) { 
                    int birdY = GROUND_Y;
                    if (birdY >= dinoTopY && birdY <= dinoBottomY) { gameOver = true; }
                }
                else if (obsType == 2) { 
                    int birdY = GROUND_Y - 1;
                    if (birdY >= dinoTopY && birdY <= dinoBottomY) { gameOver = true; }
                }
            }
            
            if (!gameOver && obsType == 0 && cacWidth == 2 && obsX == 6) {
                int cacTopY = GROUND_Y - cacHeight + 1;
                for (int y = dinoTopY; y <= dinoBottomY; y++) {
                    if (y >= cacTopY && y <= GROUND_Y) { gameOver = true; break; }
                }
            }
            if (gameOver) break;

            COORD coord = {0, 0}; SetConsoleCursorPosition(hConsole, coord);
            std::string out = "\033[0m=== Chrome Dino ===\n";
            out += "User: " + std::string(username) + " | \033[93mScore: " + std::to_string(score) + "\033[0m | Record: " + std::to_string(current_high) + " pts\n";
            out += "Current Engine Speed: \033[96m" + std::to_string(60 - (baseSleep - 16)) + "\033[0m Hz\n";
            
            for (int y = 0; y <= GROUND_Y + 1; y++) {
                out += "##"; 
                for (int x = 0; x < SCREEN_WIDTH; x++) {
                    
                    bool drawDino = false;
                    if (x == 7) {
                        if (isHoldingDuck && !isJumping) {
                            if (y == cDinoY) { out += "\033[92;1mdn\033[0m"; drawDino = true; }
                        } else {
                            if (y == cDinoY - 1) { out += "\033[92;1mDN\033[0m"; drawDino = true; }
                            else if (y == cDinoY) { out += "\033[32mdn\033[0m"; drawDino = true; }
                        }
                    }
                    
                    if (!drawDino) {
                        if (obsType == 0 && (x == obsX || (cacWidth == 2 && x == obsX + 1)) && (y <= GROUND_Y && y >= GROUND_Y - cacHeight + 1)) {
                            out += "\033[91m||\033[0m"; 
                        }
                        else if (obsType == 1 && x == obsX && y == GROUND_Y) {
                            out += "\033[96mBR\033[0m"; 
                        }
                        else if (obsType == 2 && x == obsX && y == GROUND_Y - 1) {
                            out += "\033[96mBR\033[0m"; 
                        }
                        else if (y == GROUND_Y + 1) {
                            out += "\033[90m--\033[0m";
                        }
                        else {
                            out += "  ";
                        }
                    }
                }
                out += "##\n"; 
            }
            out += "============================================================================================================\n";
            out += "Controls: [W/UP/SPACE] Jump | Hold [S/DOWN] Duck (1-Grid High) & Fast Fall | [ESC] Exit\n";
            std::cout << out;

            Sleep(baseSleep); 
        }

        std::cout << "\n\033[91mGAME OVER!\033[0m Your Score: " << score << " points.\n";
        hStats = LoadLibrary("libs\\stats.dll");
        if (hStats) {
            UpdateExtFunc updateScoreExtended = (UpdateExtFunc)GetProcAddress(hStats, "updateScoreExtended");
            if (updateScoreExtended) {
                updateScoreExtended(username, "ChromeDino", score, "pts"); 
            }
            FreeLibrary(hStats);
        }
        
        cursorInfo.bVisible = TRUE; SetConsoleCursorInfo(hConsole, &cursorInfo);
        std::cout << "Press Enter to return to menu...";
        while (_kbhit()) _getch();
        std::cin.get();
    }
}