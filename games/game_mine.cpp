#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <string>
#include <cstring> 

typedef void (*GetStatsFunc)(const char*, char*);
typedef void (*UpdateExtFunc)(const char*, const char*, int, const char*);

const int M_ROWS = 9;
const int M_COLS = 9;
const int M_MINES = 10;

int mineMap[M_ROWS][M_COLS] = {0};
bool revealed[M_ROWS][M_COLS] = {false};
bool flagged[M_ROWS][M_COLS] = {false};

std::string getNumColor(int num) {
    switch (num) {
        case 0: return "   "; 
        case 1: return " \033[94m1\033[0m "; 
        case 2: return " \033[92m2\033[0m "; 
        case 3: return " \033[91m3\033[0m "; 
        case 4: return " \033[34m4\033[0m "; 
        case 5: return " \033[31m5\033[0m "; 
        default: return " " + std::to_string(num) + " ";
    }
}

void floodReveal(int r, int c) {
    if (r < 0 || r >= M_ROWS || c < 0 || c >= M_COLS || revealed[r][c] || flagged[r][c]) return;
    revealed[r][c] = true;
    if (mineMap[r][c] == 0) {
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                floodReveal(r + dr, c + dc);
            }
        }
    }
}

extern "C" {
    __declspec(dllexport) const char* getGameName() {
        return "Minesweeper"; 
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
                char buf[64] = "GET_MINE_HIGH"; 
                getStats(username, buf);
                current_high = std::atoi(buf);
            }
            FreeLibrary(hStats);
        }

        std::srand(std::time(0));
        std::memset(mineMap, 0, sizeof(mineMap));
        std::memset(revealed, 0, sizeof(revealed));
        std::memset(flagged, 0, sizeof(flagged));

        int placedMines = 0;
        while (placedMines < M_MINES) {
            int r = std::rand() % M_ROWS;
            int c = std::rand() % M_COLS;
            if (mineMap[r][c] != -1) {
                mineMap[r][c] = -1;
                placedMines++;
            }
        }

        for (int r = 0; r < M_ROWS; r++) {
            for (int c = 0; c < M_COLS; c++) {
                if (mineMap[r][c] == -1) continue;
                int count = 0;
                for (int dr = -1; dr <= 1; dr++) {
                    for (int dc = -1; dc <= 1; dc++) {
                        int nr = r + dr, nc = c + dc;
                        if (nr >= 0 && nr < M_ROWS && nc >= 0 && nc < M_COLS && mineMap[nr][nc] == -1) {
                            count++;
                        }
                    }
                }
                mineMap[r][c] = count;
            }
        }

        int cursorR = 0, cursorC = 0;
        bool spaceLock = false, fLock = false;
        bool stateChanged = true;
        bool gameWon = false;
        bool gameLost = false;

        int wHoldTimer = 0, aHoldTimer = 0, sHoldTimer = 0, dHoldTimer = 0;
        const int HOLD_DELAY = 5; 

        std::time_t gameStartTime = std::time(0);
        const int TIME_LIMIT = 200; 
        int frameCounter = 0;

        while(_kbhit()) _getch();

        while (!gameLost && !gameWon) {
            std::time_t current_time = std::time(0);
            int timePassed = static_cast<int>(current_time - gameStartTime);
            int timeLeft = TIME_LIMIT - timePassed;

            if (timeLeft <= 0) { gameLost = true; break; }

            if (stateChanged || frameCounter % 4 == 0) { 
                COORD coord = {0, 0}; SetConsoleCursorPosition(hConsole, coord);
                std::string out = "\033[0m=== Minesweeper ===\n";
                out += "User: " + std::string(username) + " | Record: " + std::to_string(current_high) + " Clears\n";
                out += "Time Left: \033[96m" + std::to_string(timeLeft) + "\033[0m s\n"; 
                out += "---------------------------------\n";

                for (int r = 0; r < M_ROWS; r++) {
                    out += "## "; 
                    for (int c = 0; c < M_COLS; c++) {
                        bool isCursor = (r == cursorR && c == cursorC);

                        if (isCursor) {
                            if (revealed[r][c]) {
                                if (mineMap[r][c] == 0) out += "\033[30;103m[ ]\033[0m";
                                else out += "\033[30;103m[" + std::to_string(mineMap[r][c]) + "]\033[0m";
                            } 
                            else if (flagged[r][c]) out += "\033[30;103m[F]\033[0m";
                            else out += "\033[30;103m[ ]\033[0m";
                        } 
                        else {
                            if (revealed[r][c]) {
                                if (mineMap[r][c] == 0) out += "   ";
                                else out += getNumColor(mineMap[r][c]);
                            } 
                            else if (flagged[r][c]) {
                                out += "\033[91m[F]\033[0m"; 
                            } 
                            else {
                                out += "\033[90m[ ]\033[0m"; 
                            }
                        }
                        out += " "; 
                    }
                    out += "##\n";
                }
                out += "---------------------------------\n";
                out += "Controls: Hold [WASD] Smooth Move | [SPACE] Reveal | [F] Flag\n";
                std::cout << out;
                stateChanged = false;
            }

            if (GetAsyncKeyState('W') & 0x8000) { if (wHoldTimer == 0 || wHoldTimer >= HOLD_DELAY) { if (cursorR > 0) { cursorR--; stateChanged = true; } } wHoldTimer++; } else { wHoldTimer = 0; }
            if (GetAsyncKeyState('S') & 0x8000) { if (sHoldTimer == 0 || sHoldTimer >= HOLD_DELAY) { if (cursorR < M_ROWS - 1) { cursorR++; stateChanged = true; } } sHoldTimer++; } else { sHoldTimer = 0; }
            if (GetAsyncKeyState('A') & 0x8000) { if (aHoldTimer == 0 || aHoldTimer >= HOLD_DELAY) { if (cursorC > 0) { cursorC--; stateChanged = true; } } aHoldTimer++; } else { aHoldTimer = 0; }
            if (GetAsyncKeyState('D') & 0x8000) { if (dHoldTimer == 0 || dHoldTimer >= HOLD_DELAY) { if (cursorC < M_COLS - 1) { cursorC++; stateChanged = true; } } dHoldTimer++; } else { dHoldTimer = 0; }

            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                if (!spaceLock) {
                    if (!flagged[cursorR][cursorC] && !revealed[cursorR][cursorC]) {
                        if (mineMap[cursorR][cursorC] == -1) { gameLost = true; } 
                        else { floodReveal(cursorR, cursorC); }
                        stateChanged = true;
                    }
                    spaceLock = true;
                }
            } else { spaceLock = false; }

            if (GetAsyncKeyState('F') & 0x8000) {
                if (!fLock) {
                    if (!revealed[cursorR][cursorC]) { flagged[cursorR][cursorC] = !flagged[cursorR][cursorC]; stateChanged = true; }
                    fLock = true;
                }
            } else { fLock = false; }

            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;

            if (!gameLost) {
                bool winCheck = true;
                for (int r = 0; r < M_ROWS; r++) {
                    for (int c = 0; c < M_COLS; c++) { if (mineMap[r][c] != -1 && !revealed[r][c]) winCheck = false; }
                }
                if (winCheck) gameWon = true;
            }

            frameCounter++;
            Sleep(40); 
        }

        std::system("cls");
        std::time_t gameEndTime = std::time(0);
        int finalTimePassed = static_cast<int>(gameEndTime - gameStartTime);
        int finalTimeLeft = TIME_LIMIT - finalTimePassed;
        if (finalTimeLeft < 0) finalTimeLeft = 0;

        std::string finalOut = "=== Game Settled ===\n";
        for (int r = 0; r < M_ROWS; r++) {
            finalOut += "## ";
            for (int c = 0; c < M_COLS; c++) {
                if (mineMap[r][c] == -1) finalOut += "\033[91m[*]\033[0m "; 
                else if (mineMap[r][c] == 0) finalOut += "    ";
                else finalOut += getNumColor(mineMap[r][c]) + " ";
            }
            finalOut += "##\n";
        }
        std::cout << finalOut << "---------------------------------\n";

        int safeCellsRevealed = 0;
        for (int r = 0; r < M_ROWS; r++) {
            for (int c = 0; c < M_COLS; c++) { if (revealed[r][c] && mineMap[r][c] != -1) safeCellsRevealed++; }
        }

        int finalScore = safeCellsRevealed * 15; 

        if (gameWon) {
            finalScore += (finalTimeLeft * 5) + 1000;
            std::cout << "\n\033[92;1mCONGRATULATIONS! VICTORY!\033[0m\n";
            std::cout << "Time Bonus (Left " << finalTimeLeft << "s): +\033[92m" << finalTimeLeft * 5 << "\033[0m pts\n";
            std::cout << "Perfect Clear Bonus: +\033[92m1000\033[0m pts\n";
        } else if (gameLost) {
            finalScore -= 100;
            if (finalScore < 0 || finalScore >= 500) finalScore = 0;
            std::cout << "\n\033[91;1mBOOM! Game Over.\033[0m\n";
        }
        
        std::cout << "Your Total Score: \033[1;93m" << finalScore << "\033[0m points.\n";

        hStats = LoadLibrary("libs\\stats.dll");
        if (hStats) {
            UpdateExtFunc updateScoreExtended = (UpdateExtFunc)GetProcAddress(hStats, "updateScoreExtended");
            if (updateScoreExtended) {
                updateScoreExtended(username, "Minesweeper", finalScore, "wins"); 
            }
            FreeLibrary(hStats);
        }

        cursorInfo.bVisible = TRUE; 
        SetConsoleCursorInfo(hConsole, &cursorInfo);
        std::cout << "\nPress Enter to return to menu...";
        while (_kbhit()) _getch();
        std::cin.get();
    }
}