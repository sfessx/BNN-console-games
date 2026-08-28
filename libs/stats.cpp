#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <windows.h> 
#include <mutex> 
#include <map>
#include <algorithm>

struct GameRecord { int value; int plays; std::string type; };
struct UserStats { std::string username; int logins; int play_time; std::string reg_time; std::map<std::string, GameRecord> games; };

static std::mutex file_mutex;

std::vector<UserStats> loadAllStats() {
    std::vector<UserStats> list; std::ifstream file("libs\\stats.txt"); if (!file.is_open()) return list;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line); UserStats u;
        if (!(ss >> u.username >> u.logins >> u.play_time)) continue;
        std::string date_part, time_part; if (!(ss >> date_part >> time_part)) continue;
        u.reg_time = date_part + " " + time_part;
        std::string gameEntry;
        while (ss >> gameEntry) {
            std::stringstream gss(gameEntry); std::string gname, valStr, playStr, gtype;
            if (std::getline(gss, gname, ':') && 
                std::getline(gss, valStr, ':') && 
                std::getline(gss, playStr, ':') && 
                std::getline(gss, gtype, ':')) {
                u.games[gname] = { std::atoi(valStr.c_str()), std::atoi(playStr.c_str()), gtype };
            }
        }
        list.push_back(u);
    }
    file.close(); return list;
}

void saveAllStats(const std::vector<UserStats>& list) {
    std::ofstream file("libs\\stats.txt");
    for (const auto& u : list) {
        file << u.username << " " << u.logins << " " << u.play_time << " " << u.reg_time;
        for (const auto& pair : u.games) file << " " << pair.first << ":" << pair.second.value << ":" << pair.second.plays << ":" << pair.second.type;
        file << "\n";
    }
    file.close();
}

extern "C" {
    __declspec(dllexport) void recordLogin(const char* username) {
        std::string target(username); if (target == "Guest") return;
        std::lock_guard<std::mutex> lock(file_mutex); auto list = loadAllStats(); bool found = false;
        for (auto& u : list) { if (u.username == target) { u.logins++; found = true; break; } }
        if (!found) {
            std::time_t now = std::time(0); char time_str[64] = {0}; 
            std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
            list.push_back({target, 1, 0, std::string(time_str), {}}); 
        }
        saveAllStats(list);
    } 

    __declspec(dllexport) void deleteAccountData(const char* username) {
        std::lock_guard<std::mutex> lock(file_mutex); auto list = loadAllStats(); std::vector<UserStats> remain_stats;
        for (const auto& u : list) { if (u.username != username) remain_stats.push_back(u); }
        saveAllStats(remain_stats);
    }

    __declspec(dllexport) void showProfile(const char* username, int current_session_sec) {
        if (std::string(username) == "Guest") {
            std::system("cls");
            std::cout << "=== USER PROFILE ===\nUsername : " << username << "\nPermission: Temporary Guest\n";
            int d = current_session_sec / 86400; int h = (current_session_sec % 86400) / 3600; int m = (current_session_sec % 3600) / 60; int s_rem = current_session_sec % 60;
            std::cout << "Session  : " << d << "D" << h << "H" << m << "M" << s_rem << "S\n====================\n\nPress Enter to return...";
            std::cin.get(); return;
        }

        std::vector<UserStats> list;
        { std::lock_guard<std::mutex> lock(file_mutex); list = loadAllStats(); }
        
        for (auto& u : list) {
            if (u.username == username) {
                u.play_time += current_session_sec;
                
                std::vector<std::pair<std::string, GameRecord>> gVector;
                for (const auto& pair : u.games) gVector.push_back(pair);

                int profilePage = 0;
                const int RECORDS_PER_PAGE = 4;
                int totalRecords = gVector.size();
                int totalPages = (totalRecords + RECORDS_PER_PAGE - 1) / RECORDS_PER_PAGE;
                if (totalPages == 0) totalPages = 1;

                while (true) {
                    std::system("cls");
                    std::cout << "=== USER PROFILE (Page " << profilePage + 1 << " / " << totalPages << ") ===\n";
                    std::cout << "Username   : " << u.username << "\n";
                    std::cout << "Logins     : " << u.logins << " times\n";
                    std::cout << "Registered : " << u.reg_time << "\n\n";

                    int start = profilePage * RECORDS_PER_PAGE;
                    int end = std::min(start + RECORDS_PER_PAGE, totalRecords);

                    if (totalRecords == 0) {
                        std::cout << " No game history recorded yet.\n\n";
                    }

                    for (int i = start; i < end; i++) {
                        std::cout << "[" << gVector[i].first << "]\n";
                        if (gVector[i].second.type == "wins") {
                            std::cout << " Total Clears: " << gVector[i].second.value << " times\n";
                        } else {
                            std::cout << " HighScore   : " << gVector[i].second.value << " pts\n";
                        }
                        std::cout << " Total Plays : " << gVector[i].second.plays << " times\n\n";
                    }

                    int t = u.play_time;
                    std::cout << "Total Time : " << t/86400 << "D" << (t%86400)/3600 << "H" << (t%3600)/60 << "M" << t%60 << "S\n";
                    std::cout << "=========================================\n";
                    
                    std::cout << "Navigation: ";
                    if (totalPages > 1) {
                        std::cout << "[N] Next Page | [P] Prev Page | ";
                    }
                    std::cout << "[E] Exit Profile Page\n";
                    std::cout << "Select command: ";

                    std::string choiceStr;
                    std::getline(std::cin, choiceStr);
                    if (choiceStr.empty()) continue;

                    std::string cmd = choiceStr;
                    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

                    if (cmd == "E") {
                        break; 
                    }
                    if (totalPages > 1) {
                        if (cmd == "N") { 
                            profilePage = (profilePage + 1) % totalPages; 
                            continue; 
                        }
                        if (cmd == "P") { 
                            profilePage = (profilePage - 1 + totalPages) % totalPages; 
                            continue; 
                        }
                    }
                }
                { std::lock_guard<std::mutex> lock(file_mutex); saveAllStats(list); }
                return;
            }
        }
    }

    __declspec(dllexport) void getStats(const char* username, char* result) {
        std::string target(username); if (target == "Guest") return;
        std::string req(result); std::lock_guard<std::mutex> lock(file_mutex); auto list = loadAllStats();
        std::string targetGame = "";
        if (req == "GET_GUESS_HIGH")  targetGame = "GuessGame";
        if (req == "GET_SNAKE_HIGH")  targetGame = "SnakeGame";
        if (req == "GET_TETRIS_HIGH") targetGame = "TetrisGame";
        if (req == "GET_2048_HIGH")   targetGame = "2048Game";
        if (req == "GET_FLAPPY_HIGH") targetGame = "FlappyBird";
        if (req == "GET_MINE_HIGH")   targetGame = "Minesweeper";

        for (const auto& u : list) {
            if (u.username == target) {
                if (!targetGame.empty()) {
                    auto it = u.games.find(targetGame);
                    if (it != u.games.end()) std::sprintf(result, "%d", it->second.value);
                    else std::sprintf(result, "0");
                    return;
                }
            }
        }
        std::sprintf(result, "0");
    } 

    __declspec(dllexport) void updateScoreExtended(const char* username, const char* gameName, int score, const char* metricType) {
        std::string target(username); if (target == "Guest" || !gameName || !metricType) return;
        std::lock_guard<std::mutex> lock(file_mutex); auto list = loadAllStats();
        for (auto& u : list) {
            if (u.username == target) {
                std::string gname(gameName); std::string mtype(metricType);
                if (u.games.find(gname) == u.games.end()) u.games[gname] = { 0, 0, mtype };
                u.games[gname].plays++; 
                if (mtype == "wins") { if (score >= 500) u.games[gname].value += 1; } 
                else { if (score > u.games[gname].value) u.games[gname].value = score; }
                break;
            }
        }
        saveAllStats(list);
    } 
}