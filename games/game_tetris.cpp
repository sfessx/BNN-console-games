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

const int BOARD_WIDTH = 10;
const int BOARD_HEIGHT = 18;

int blocks[7][4][4] = {
    { {0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0} },
    { {1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0} },
    { {0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0} },
    { {0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0} },
    { {0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0} },
    { {0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0} },
    { {1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0} }
};

int board[BOARD_HEIGHT][BOARD_WIDTH] = {0};
int currentBlock[4][4];
int blockX = 3, blockY = 0;
int currentType = 0;
int nextType = 0; 
int score = 0;

std::string getTileColor(int val) {
    if (val == 0) return "  ";
    switch (val) {
        case 1: return "\033[96m[]\033[0m";
        case 2: return "\033[91m[]\033[0m";
        case 3: return "\033[94m[]\033[0m";
        case 4: return "\033[93m[]\033[0m";
        case 5: return "\033[92m[]\033[0m";
        case 6: return "\033[95m[]\033[0m";
        case 7: return "\033[35m[]\033[0m";
        default: return "[]";
    }
}

void rotateBlock() {
    int temp[4][4] = {0};
    for(int i=0; i<4; i++) for(int j=0; j<4; j++) temp[j][3-i] = currentBlock[i][j];
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            if(temp[i][j]) {
                int nextX = blockX + j; int nextY = blockY + i;
                if(nextX < 0 || nextX >= BOARD_WIDTH || nextY >= BOARD_HEIGHT) return;
                if(nextY >= 0 && board[nextY][nextX]) return;
            }
        }
    }
    std::memcpy(currentBlock, temp, sizeof(temp));
}

bool checkCollision(int nextX, int nextY) {
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            if(currentBlock[i][j]) {
                int boardX = nextX + j; int boardY = nextY + i;
                if(boardX < 0 || boardX >= BOARD_WIDTH || boardY >= BOARD_HEIGHT) return true;
                if(boardY >= 0 && board[boardY][boardX]) return true;
            }
        }
    }
    return false;
}

void lockBlock() {
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            if(currentBlock[i][j] && (blockY + i) >= 0) {
                board[blockY + i][blockX + j] = currentType + 1; 
            }
        }
    }
}

void clearLines() {
    for(int i = BOARD_HEIGHT - 1; i >= 0; i--) {
        bool full = true;
        for(int j = 0; j < BOARD_WIDTH; j++) { if(board[i][j] == 0) { full = false; break; } }
        if(full) {
            score += 100;
            for(int k = i; k > 0; k--) std::memcpy(board[k], board[k-1], sizeof(board[k]));
            std::memset(board[0], 0, sizeof(board[0]));
            i++; 
        }
    }
}

extern "C" {
    __declspec(dllexport) const char* getGameName() {
        return "Tetris Game";
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
                char buf[64] = "GET_TETRIS_HIGH"; getStats(username, buf);
                current_high = std::atoi(buf);
            }
            FreeLibrary(hStats);
        }

        std::srand(std::time(0));
        std::memset(board, 0, sizeof(board));
        score = 0; bool gameOver = false;

        currentType = std::rand() % 7;
        nextType = std::rand() % 7; 
        std::memcpy(currentBlock, blocks[currentType], sizeof(currentBlock));
        blockX = 3; blockY = -1;
        
        int dropTimer = 0;
        bool wKeyLocked = false; bool spaceKeyLocked = false;

        int aHoldTimer = 0; int dHoldTimer = 0;
        const int HOLD_DELAY = 3; 

        while(_kbhit()) _getch();

        while(!gameOver) {
            if (GetAsyncKeyState('A') & 0x8000) {
                if (aHoldTimer == 0 || aHoldTimer >= HOLD_DELAY) {
                    if(!checkCollision(blockX - 1, blockY)) blockX--;
                }
                aHoldTimer++;
            } else { aHoldTimer = 0; }

            if (GetAsyncKeyState('D') & 0x8000) {
                if (dHoldTimer == 0 || dHoldTimer >= HOLD_DELAY) {
                    if(!checkCollision(blockX + 1, blockY)) blockX++;
                }
                dHoldTimer++;
            } else { dHoldTimer = 0; }

            if (GetAsyncKeyState('S') & 0x8000) { 
                if(!checkCollision(blockX, blockY + 1)) blockY++; 
            }

            if (GetAsyncKeyState('W') & 0x8000) {
                if (!wKeyLocked) { rotateBlock(); wKeyLocked = true; }
            } else { wKeyLocked = false; }

            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                if (!spaceKeyLocked) {
                    while(!checkCollision(blockX, blockY + 1)) { blockY++; }
                    lockBlock(); clearLines(); 
                    currentType = nextType; nextType = std::rand() % 7;
                    std::memcpy(currentBlock, blocks[currentType], sizeof(currentBlock));
                    blockX = 3; blockY = -1; dropTimer = 0; spaceKeyLocked = true;
                    if(checkCollision(blockX, blockY)) { gameOver = true; }
                    continue;
                }
            } else { spaceKeyLocked = false; }

            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { break; }

            dropTimer++;
            if(dropTimer >= 6) {
                dropTimer = 0;
                if(!checkCollision(blockX, blockY + 1)) { blockY++; } 
                else {
                    if(blockY < 0) { gameOver = true; } 
                    else { 
                        lockBlock(); clearLines(); 
                        currentType = nextType; nextType = std::rand() % 7;
                        std::memcpy(currentBlock, blocks[currentType], sizeof(currentBlock)); 
                        blockX = 3; blockY = -1; 
                        if(checkCollision(blockX, blockY)) { gameOver = true; }
                    }
                }
            }

            COORD coord = {0, 0}; SetConsoleCursorPosition(hConsole, coord);
            std::string out = "\033[0m=== Tetris Game ===\n";
            
            for(int i=0; i<BOARD_HEIGHT; i++) {
                out += "##"; 
                for(int j=0; j<BOARD_WIDTH; j++) {
                    bool isCurrent = false;
                    if(i >= blockY && i < blockY + 4 && j >= blockX && j < blockX + 4) { 
                        if(currentBlock[i - blockY][j - blockX]) isCurrent = true; 
                    }
                    if(isCurrent) out += getTileColor(currentType + 1);
                    else out += getTileColor(board[i][j]);
                }
                out += "##"; 

                if (i == 1)      out += "   PLAYER   : " + std::string(username);
                else if (i == 2) out += "   SCORE    : \033[93m" + std::to_string(score) + "\033[0m";
                else if (i == 3) out += "   RECORD   : " + std::to_string(current_high) + " pts";
                else if (i == 5) out += "   [NEXT]   ";
                else if (i >= 6 && i <= 9) {
                    out += "   ";
                    int nextLine = i - 6;
                    for (int nextX = 0; nextX < 4; nextX++) {
                        if (blocks[nextType][nextLine][nextX]) out += getTileColor(nextType + 1); 
                        else out += "  ";
                    }
                }
                out += "\n";
            }
            out += "============================\n";
            std::cout << out;
            Sleep(50); 
        }

        while ((GetAsyncKeyState(VK_SPACE) & 0x8000) || (GetAsyncKeyState(VK_ESCAPE) & 0x8000)) { Sleep(10); }
        FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
        while (_kbhit()) _getch();

        std::cout << "\n\033[91mGAME OVER!\033[0m Your Score: " << score << " points.\n";
        
        hStats = LoadLibrary("libs\\stats.dll");
        if (hStats) {
            UpdateExtFunc updateScoreExtended = (UpdateExtFunc)GetProcAddress(hStats, "updateScoreExtended");
            if (updateScoreExtended) {
                updateScoreExtended(username, "TetrisGame", score, "pts"); 
            }
            FreeLibrary(hStats);
        }
        
        cursorInfo.bVisible = TRUE; SetConsoleCursorInfo(hConsole, &cursorInfo);
        std::cout << "Press Enter to return..."; std::cin.get();
    }
}