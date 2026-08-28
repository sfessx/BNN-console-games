#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <string>

typedef void (*GetStatsFunc)(const char*, char*);
typedef void (*UpdateExtFunc)(const char*, const char*, int, const char*);

const int WIDTH = 20;
const int HEIGHT = 15;

struct Point { int x, y; };

extern "C" {
    __declspec(dllexport) const char* getGameName() {
        return "Snake Game";
    }

    __declspec(dllexport) void playGame(const char* username) {
        std::system("cls");
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        
        DWORD dwMode = 0; GetConsoleMode(hConsole, &dwMode);
        SetConsoleMode(hConsole, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hConsole, &cursorInfo);
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(hConsole, &cursorInfo);

        int current_high = 0;
        HMODULE hStats = LoadLibrary("libs\\stats.dll");
        if (hStats) {
            GetStatsFunc getStats = (GetStatsFunc)GetProcAddress(hStats, "getStats");
            if (getStats) {
                char buf[64] = "GET_SNAKE_HIGH";
                getStats(username, buf);
                current_high = std::atoi(buf);
            }
            FreeLibrary(hStats);
        }

        std::srand(std::time(0));
        bool gameOver = false;
        std::vector<Point> snake;
        snake.push_back({WIDTH / 2, HEIGHT / 2});
        snake.push_back({WIDTH / 2, HEIGHT / 2 + 1});
        snake.push_back({WIDTH / 2, HEIGHT / 2 + 2});

        int dirX = 0, dirY = -1; 
        Point food = { std::rand() % (WIDTH - 2) + 1, std::rand() % (HEIGHT - 2) + 1 };
        int score = 0;

        while (!gameOver) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == 27) break;
                switch (ch) {
                    case 'w': case 'W': if (dirY != 1)  { dirX = 0;  dirY = -1; } break;
                    case 's': case 'S': if (dirY != -1) { dirX = 0;  dirY = 1;  } break;
                    case 'a': case 'A': if (dirX != 1)  { dirX = -1; dirY = 0;  } break;
                    case 'd': case 'D': if (dirX != -1) { dirX = 1;  dirY = 0;  } break;
                }
            }

            Point newHead = {snake[0].x + dirX, snake[0].y + dirY};
            if (newHead.x <= 0) newHead.x = WIDTH - 2; else if (newHead.x >= WIDTH - 1) newHead.x = 1;
            if (newHead.y <= 0) newHead.y = HEIGHT - 2; else if (newHead.y >= HEIGHT - 1) newHead.y = 1;

            for (size_t i = 1; i < snake.size(); i++) {
                if (newHead.x == snake[i].x && newHead.y == snake[i].y) { gameOver = true; break; }
            }
            if (gameOver) break;

            snake.insert(snake.begin(), newHead);

            if (newHead.x == food.x && newHead.y == food.y) {
                score += 20; 
                bool onSnake;
                do {
                    onSnake = false;
                    food.x = std::rand() % (WIDTH - 2) + 1;
                    food.y = std::rand() % (HEIGHT - 2) + 1;
                    for (const auto& part : snake) { if (food.x == part.x && food.y == part.y) { onSnake = true; break; } }
                } while (onSnake);
            } else {
                snake.pop_back();
            }

            COORD coord = {0, 0};
            SetConsoleCursorPosition(hConsole, coord);
            
            std::string buffer = "\033[0m=== Snake Game ===\nUser: " + std::string(username) + " | \033[93mScore: " + std::to_string(score) + "\033[0m | Record: " + std::to_string(current_high) + "\n";
            for (int y = 0; y < HEIGHT; y++) {
                for (int x = 0; x < WIDTH; x++) {
                    if (y == 0 || y == HEIGHT - 1 || x == 0 || x == WIDTH - 1) {
                        buffer += "\033[93m#\033[0m"; 
                    }
                    else if (x == snake[0].x && y == snake[0].y) {
                        buffer += "\033[92;1mO\033[0m"; 
                    }
                    else {
                        bool isBody = false;
                        for (size_t i = 1; i < snake.size(); i++) { 
                            if (x == snake[i].x && y == snake[i].y) { 
                                buffer += "\033[32mo\033[0m"; 
                                isBody = true; 
                                break; 
                            } 
                        }
                        if (!isBody) { 
                            if (x == food.x && y == food.y) {
                                buffer += "\033[91;1m*\033[0m"; 
                            } else {
                                buffer += ' '; 
                            }
                        }
                    }
                }
                buffer += '\n';
            }
            std::cout << buffer;
            Sleep(100); 
        }

        std::cout << "\n\033[91mGAME OVER!\033[0m Your Score: " << score << " points.\n";
        
        hStats = LoadLibrary("libs\\stats.dll");
        if (hStats) {
            UpdateExtFunc updateScoreExtended = (UpdateExtFunc)GetProcAddress(hStats, "updateScoreExtended");
            if (updateScoreExtended) {
                updateScoreExtended(username, "SnakeGame", score, "pts"); 
            }
            FreeLibrary(hStats);
        }
        
        cursorInfo.bVisible = TRUE;
        SetConsoleCursorInfo(hConsole, &cursorInfo);
        std::cout << "Press Enter to return...";
        std::cin.get();
    }
}