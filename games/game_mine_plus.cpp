#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <string>
#include <cstring>
#include <algorithm>

typedef void (*GetStatsFunc)(const char*, char*);
typedef void (*UpdateExtFunc)(const char*, const char*, int, const char*);

const int MP_ROWS = 18;
const int MP_COLS = 18;
const int MP_MINES = 40;

int mineMapPlus[MP_ROWS][MP_COLS] = {0};
bool revealedPlus[MP_ROWS][MP_COLS] = {false};
bool flaggedPlus[MP_ROWS][MP_COLS] = {false};

std::string getNumColorPlus(int num) {
    switch (num) {
        case 0: return "   ";
        case 1: return " \033[94m1\033[0m ";
        case 2: return " \033[92m2\033[0m ";
        case 3: return " \033[91m3\033[0m ";
        case 4: return " \033[34m4\033[0m ";
        case 5: return " \033[31m5\033[0m ";
        case 6: return " \033[96m6\033[0m ";
        case 7: return " \033[95m7\033[0m ";
        default: return " " + std::to_string(num) + " ";
    }
}

void floodRevealPlus(int r, int c) {
    if (r < 0 || r >= MP_ROWS || c < 0 || c >= MP_COLS || revealedPlus[r][c] || flaggedPlus[r][c]) return;
    revealedPlus[r][c] = true;
    if (mineMapPlus[r][c] == 0) {
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                floodRevealPlus(r + dr, c + dc);
            }
        }
    }
}

extern "C" {
    __declspec(dllexport) const char* getGameName() {
        return "Minesweeper Plus";
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
                char buf[64] = "GET_MINE_PLUS_HIGH";
                getStats(username, buf);
                current_high = std::atoi(buf);
            }
            FreeLibrary(hStats);
        }

        std::srand(std::time(0));
        std::memset(mineMapPlus, 0, sizeof(mineMapPlus));
        std::memset(revealedPlus, 0, sizeof(revealedPlus));
        std::memset(flaggedPlus, 0, sizeof(flaggedPlus));

        int placedMines = 0;
        while (placedMines < MP_MINES) {
            int r = std::rand() % MP_ROWS;
            int c = std::rand() % MP_COLS;
            if (mineMapPlus[r][c] != -1) {
                mineMapPlus[r][c] = -1;
                placedMines++;
            }
        }

        for (int r = 0; r < MP_ROWS; r++) {
            for (int c = 0; c < MP_COLS; c++) {
                if (mineMapPlus[r][c] == -1) continue;
                int count = 0;
                for (int dr = -1; dr <= 1; dr++) {
                    for (int dc = -1; dc <= 1; dc++) {
                        int nr = r + dr, nc = c + dc;
                        if (nr >= 0 && nr < MP_ROWS && nc >= 0 && nc < MP_COLS && mineMapPlus[nr][nc] == -1) {
                            count++;
                        }
                    }
                }
                mineMapPlus[r][c] = count;
            }
        }

        int cursorR = 0, cursorC = 0;
        bool spaceLock = false, fLock = false;
        bool stateChanged = true;
        bool gameWon = false;
        bool gameLost = false;

        int wHoldTimer = 0, aHoldTimer = 0, sHoldTimer = 0, dHoldTimer = 0;
        const int HOLD_DELAY = 4;

        std::time_t gameStartTime = std::time(0);
        int frameCounter = 0;

        while(_kbhit()) _getch();

        while (!gameLost && !gameWon) {
            std::time_t current_time = std::time(0);
            int timePassed = static_cast<int>(current_time - gameStartTime);

            if (stateChanged || frameCounter % 4 == 0) {
                COORD coord = {0, 0}; SetConsoleCursorPosition(hConsole, coord);
                std::string out = "\033[0m=== Minesweeper Plus (18x18) ===\n";
                out += "User: " + std::string(username) + " | Record: " + std::to_string(current_high) + " Clears\n";
                out += "Elapsed Time: \033[96m" + std::to_string(timePassed) + "\033[0m s\n";
                out += "---------------------------------------------------------------------------------------\n";

                for (int r = 0; r < MP_ROWS; r++) {
                    out += "## ";
                    for (int c = 0; c < MP_COLS; c++) {
                        bool isCursor = (r == cursorR && c == cursorC);

                        if (isCursor) {
                            if (revealedPlus[r][c]) {
                                if (mineMapPlus[r][c] == 0) out += "\033[30;103m[ ]\033[0m";
                                else out += "\033[30;103m[" + std::to_string(mineMapPlus[r][c]) + "]\033[0m";
                            }
                            else if (flaggedPlus[r][c]) out += "\033[30;103m[F]\033[0m";
                            else out += "\033[30;103m[ ]\033[0m";
                        }
                        else {
                            if (revealedPlus[r][c]) {
                                if (mineMapPlus[r][c] == 0) out += "   ";
                                else out += getNumColorPlus(mineMapPlus[r][c]);
                            }
                            else if (flaggedPlus[r][c]) {
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
                out += "---------------------------------------------------------------------------------------\n";
                out += "Controls: Hold [WASD] Move | [SPACE] Reveal | [F] Flag | [ESC] Exit\n";
                std::cout << out;
                stateChanged = false;
            }

            if (GetAsyncKeyState('W') & 0x8000) { if (wHoldTimer == 0 || wHoldTimer >= HOLD_DELAY) { if (cursorR > 0) { cursorR--; stateChanged = true; } } wHoldTimer++; } else { wHoldTimer = 0; }
            if (GetAsyncKeyState('S') & 0x8000) { if (sHoldTimer == 0 || sHoldTimer >= HOLD_DELAY) { if (cursorR < MP_ROWS - 1) { cursorR++; stateChanged = true; } } sHoldTimer++; } else { sHoldTimer = 0; }
            if (GetAsyncKeyState('A') & 0x8000) { if (aHoldTimer == 0 || aHoldTimer >= HOLD_DELAY) { if (cursorC > 0) { cursorC--; stateChanged = true; } } aHoldTimer++; } else { aHoldTimer = 0; }
            if (GetAsyncKeyState('D') & 0x8000) { if (dHoldTimer == 0 || dHoldTimer >= HOLD_DELAY) { if (cursorC < MP_COLS - 1) { cursorC++; stateChanged = true; } } dHoldTimer++; } else { dHoldTimer = 0; }

            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                if (!spaceLock) {
                    if (!flaggedPlus[cursorR][cursorC] && !revealedPlus[cursorR][cursorC]) {
                        if (mineMapPlus[cursorR][cursorC] == -1) { gameLost = true; }
                        else { floodRevealPlus(cursorR, cursorC); }
                        stateChanged = true;
                    }
                    spaceLock = true;
                }
            } else { spaceLock = false; }

            if (GetAsyncKeyState('F') & 0x8000) {
                if (!fLock) {
                    if (!revealedPlus[cursorR][cursorC]) { flaggedPlus[cursorR][cursorC] = !flaggedPlus[cursorR][cursorC]; stateChanged = true; }
                    fLock = true;
                }
            } else { fLock = false; }

            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;

            if (!gameLost) {
                bool winCheck = true;
                for (int r = 0; r < MP_ROWS; r++) {
                    for (int c = 0; c < MP_COLS; c++) { if (mineMapPlus[r][c] != -1 && !revealedPlus[r][c]) winCheck = false; }
                }
                if (winCheck) gameWon = true;
            }

            frameCounter++;
            Sleep(40);
        }

        std::system("cls");
        std::time_t gameEndTime = std::time(0);
        int finalTimePassed = static_cast<int>(gameEndTime - gameStartTime);

        std::string finalOut = "=== Minesweeper Plus Settled ===\n";
        for (int r = 0; r < MP_ROWS; r++) {
            finalOut += "## ";
            for (int c = 0; c < MP_COLS; c++) {
                if (mineMapPlus[r][c] == -1) finalOut += "\033[91m[*]\033[0m ";
                else if (mineMapPlus[r][c] == 0) finalOut += "    ";
                else finalOut += getNumColorPlus(mineMapPlus[r][c]) + " ";
            }
            finalOut += "##\n";
        }
        std::cout << finalOut << "-------------------------------------------------------------\n";

        int safeCellsRevealed = 0;
        for (int r = 0; r < MP_ROWS; r++) {
            for (int c = 0; c < MP_COLS; c++) {
                if (revealedPlus[r][c] && mineMapPlus[r][c] != -1) safeCellsRevealed++;
            }
        }

        int finalScore = safeCellsRevealed * 10;

        if (gameWon) {
            int timeBonus = 3000 - (finalTimePassed * 5);
            if (timeBonus < 0) timeBonus = 0;

            finalScore += timeBonus + 1000;
            std::cout << "\n\033[92;1mCONGRATULATIONS! YOU CLEAR ALL MINES SUCCESSFULLY!\033[0m\n";
            std::cout << "Clear Time: " << finalTimePassed << " seconds\n";
            std::cout << "Time Efficiency Bonus: +\033[92m" << timeBonus << "\033[0m pts\n";
            int grandClear = 1000;
            std::cout << "Grand Clear Bonus: +\033[92m" << grandClear << "\033[0m pts\n";
        } else if (gameLost) {
            finalScore -= 200;
            if (finalScore < 0 || finalScore >= 500) finalScore = 0;
            std::cout << "\n\033[91mBOOM! You stepped on a mine. GAME OVER.\033[0m\n";
        }

        std::cout << "Your Total Score: \033[1;93m" << finalScore << "\033[0m points.\n";

        hStats = LoadLibrary("libs\\stats.dll");
        if (hStats) {
            UpdateExtFunc updateScoreExtended = (UpdateExtFunc)GetProcAddress(hStats, "updateScoreExtended");
            if (updateScoreExtended) {
                updateScoreExtended(username, "MinesweeperPlus", finalScore, "wins");
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