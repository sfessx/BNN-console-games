#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <string>
#include <cstring>
#include <iomanip>

typedef void (*GetStatsFunc)(const char*, char*);
typedef void (*UpdateExtFunc)(const char*, const char*, int, const char*);

const int G_SIZE = 4;
int grid[G_SIZE][G_SIZE] = {0};
int score = 0;

std::string getColor(int val) {
    switch (val) {
        case 2:      return "\033[97;44m";
        case 4:      return "\033[97;42m";
        case 8:      return "\033[97;43m";
        case 16:     return "\033[97;41m";
        case 32:     return "\033[97;45m";
        case 64:     return "\033[97;46m";
        case 128:    return "\033[30;103m";
        case 256:    return "\033[30;102m";
        case 512:    return "\033[30;106m";
        case 1024:   return "\033[30;105m";
        case 2048:   return "\033[37;101m";
        case 4096:   return "\033[93;41m";
        case 8192:   return "\033[96;45m";
        case 16384:  return "\033[92;44m";
        case 32768:  return "\033[91;100m";
        case 65536:  return "\033[95;107m";
        case 131072: return "\033[97;101m";
        default:     return "\033[1;93;40m";
    }
}

void spawnNumber() {
    std::vector<std::pair<int, int>> emptyCells;
    for (int i = 0; i < G_SIZE; i++) {
        for (int j = 0; j < G_SIZE; j++) {
            if (grid[i][j] == 0) emptyCells.push_back({i, j});
        }
    }
    if (!emptyCells.empty()) {
        int idx = std::rand() % emptyCells.size();
        grid[emptyCells[idx].first][emptyCells[idx].second] = (std::rand() % 10 == 0) ? 4 : 2;
    }
}

bool canMove() {
    for (int i = 0; i < G_SIZE; i++) {
        for (int j = 0; j < G_SIZE; j++) {
            if (grid[i][j] == 0) return true;
            if (i < G_SIZE - 1 && grid[i][j] == grid[i + 1][j]) return true;
            if (j < G_SIZE - 1 && grid[i][j] == grid[i][j + 1]) return true;
        }
    }
    return false;
}

bool slideLeft() {
    bool moved = false;
    for (int i = 0; i < G_SIZE; i++) {
        int temp[G_SIZE] = {0};
        int pos = 0;
        for (int j = 0; j < G_SIZE; j++) {
            if (grid[i][j] != 0) temp[pos++] = grid[i][j];
        }
        for (int j = 0; j < G_SIZE - 1; j++) {
            if (temp[j] != 0 && temp[j] == temp[j + 1]) {
                temp[j] *= 2;
                score += temp[j];
                temp[j + 1] = 0;
                moved = true;
            }
        }
        int finalRow[G_SIZE] = {0};
        int finalPos = 0;
        for (int j = 0; j < G_SIZE; j++) {
            if (temp[j] != 0) finalRow[finalPos++] = temp[j];
        }
        for (int j = 0; j < G_SIZE; j++) {
            if (grid[i][j] != finalRow[j]) moved = true;
            grid[i][j] = finalRow[j];
        }
    }
    return moved;
}

void rotateGrid() {
    int temp[G_SIZE][G_SIZE];
    for (int i = 0; i < G_SIZE; i++) {
        for (int j = 0; j < G_SIZE; j++) temp[j][G_SIZE - 1 - i] = grid[i][j];
    }
    std::memcpy(grid, temp, sizeof(grid));
}

bool moveGrid(char dir) {
    bool moved = false;
    if (dir == 'A') { moved = slideLeft(); }
    else if (dir == 'S') { rotateGrid(); moved = slideLeft(); rotateGrid(); rotateGrid(); rotateGrid(); }
    else if (dir == 'D') { rotateGrid(); rotateGrid(); moved = slideLeft(); rotateGrid(); rotateGrid(); }
    else if (dir == 'W') { rotateGrid(); rotateGrid(); rotateGrid(); moved = slideLeft(); rotateGrid(); }
    return moved;
}

extern "C" {
    __declspec(dllexport) const char* getGameName() {
        return "2048 Game";
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
                char buf[64] = "GET_2048_HIGH"; 
                getStats(username, buf);
                current_high = std::atoi(buf);
            }
            FreeLibrary(hStats);
        }

        std::srand(std::time(0));
        std::memset(grid, 0, sizeof(grid));
        score = 0;
        
        spawnNumber();
        spawnNumber();

        bool wLock = false, aLock = false, sLock = false, dLock = false;
        bool drawRequired = true;

        while (true) {
            if (drawRequired) {
                COORD coord = {0, 0}; SetConsoleCursorPosition(hConsole, coord);
                std::cout << "\033[0m=== 2048 Game ===\n";
                std::cout << "User: " << username << " | \033[93mScore: " << score << "\033[0m | Record: " << current_high << "\n";
                std::cout << "-------------------------------------\n";
                
                for (int i = 0; i < G_SIZE; i++) {
                    std::cout << "-------------------------------------\n";
                    for (int j = 0; j < G_SIZE; j++) {
                        std::cout << "|";
                        if (grid[i][j] == 0) {
                            std::cout << "      ";
                        } else {
                            std::cout << getColor(grid[i][j]) << std::setw(6) << grid[i][j] << "\033[0m";
                        }
                    }
                    std::cout << "|\n";
                }
                std::cout << "-------------------------------------\n";
                std::cout << "(W/A/S/D) Move | (ESC) Exit Game\n";
                drawRequired = false;
            }

            if (!canMove()) {
                std::cout << "\n\033[91mGAME OVER! No more available moves.\033[0m\n";
                break;
            }

            bool triggered = false;
            char moveDir = ' ';

            if (GetAsyncKeyState('W') & 0x8000) { if (!wLock) { moveDir = 'W'; wLock = true; triggered = true; } } else { wLock = false; }
            if (GetAsyncKeyState('A') & 0x8000) { if (!aLock) { moveDir = 'A'; aLock = true; triggered = true; } } else { aLock = false; }
            if (GetAsyncKeyState('S') & 0x8000) { if (!sLock) { moveDir = 'S'; sLock = true; triggered = true; } } else { sLock = false; }
            if (GetAsyncKeyState('D') & 0x8000) { if (!dLock) { moveDir = 'D'; dLock = true; triggered = true; } } else { dLock = false; }
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { break; }

            if (triggered) {
                if (moveGrid(moveDir)) {
                    spawnNumber();
                    drawRequired = true;
                }
            }
            Sleep(30);
        }

        hStats = LoadLibrary("libs\\stats.dll");
        if (hStats) {
            UpdateExtFunc updateScoreExtended = (UpdateExtFunc)GetProcAddress(hStats, "updateScoreExtended");
            if (updateScoreExtended) {
                updateScoreExtended(username, "2048Game", score, "pts"); 
            }
            FreeLibrary(hStats);
        }

        cursorInfo.bVisible = TRUE; SetConsoleCursorInfo(hConsole, &cursorInfo);
        std::cout << "\nPress Enter to return to menu...";
        while (_kbhit()) _getch();
        std::cin.get();
    }
}