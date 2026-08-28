#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <windows.h>
#include <algorithm>
#include <map>
#include <set>

struct GameRecord { int value; int plays; std::string type; };
struct UserStats { std::string username; int logins; int play_time; std::string reg_time; std::map<std::string, GameRecord> games; };

std::vector<UserStats> loadStats() {
    std::vector<UserStats> list; std::ifstream file("libs\\stats.txt"); if (!file.is_open()) return list;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line); UserStats u; std::string d_part, t_part;
        if (!(ss >> u.username >> u.logins >> u.play_time >> d_part >> t_part)) continue;
        u.reg_time = d_part + " " + t_part;
        std::string gameEntry;
        while (ss >> gameEntry) {
            std::stringstream gss(gameEntry); std::string gname, valStr, playStr, gtype;
            if (std::getline(gss, gname, ':') && std::getline(gss, valStr, ':') && std::getline(gss, playStr, ':') && std::getline(gss, gtype, ':')) {
                u.games[gname] = { std::atoi(valStr.c_str()), std::atoi(playStr.c_str()), gtype };
            }
        }
        list.push_back(u);
    }
    file.close(); return list;
}

extern "C" {
    __declspec(dllexport) const char* getLibMenuName() { return "View System Leaderboard"; }

    __declspec(dllexport) void executeFunction(const char* username) {
        auto users = loadStats();
        
        std::set<std::string> allGameNames;
        for (const auto& u : users) { for (const auto& pair : u.games) allGameNames.insert(pair.first); }

        std::vector<std::string> gNames(allGameNames.begin(), allGameNames.end());
        int totalGames = gNames.size();
        
        if (totalGames == 0) {
            std::system("cls");
            std::cout << "=========================================\n          SYSTEM LEADERBOARD             \n=========================================\n\n No records found in the system yet.\n\nPress Enter to return...";
            std::cin.get(); return;
        }

        int leaderboardPage = 0;
        const int BOARDS_PER_PAGE = 6;
        int totalPages = (totalGames + BOARDS_PER_PAGE - 1) / BOARDS_PER_PAGE;
        if (totalPages == 0) totalPages = 1;

        while (true) {
            std::system("cls");
            std::cout << "=========================================\n";
            std::cout << "      SYSTEM LEADERBOARD (Page " << leaderboardPage + 1 << " / " << totalPages << ")     \n";
            std::cout << "=========================================\n\n";

            int start = leaderboardPage * BOARDS_PER_PAGE;
            int end = std::min(start + BOARDS_PER_PAGE, totalGames);

            for (int k = start; k < end; k++) {
                std::string gameName = gNames[k];
                std::cout << "--- [ " << gameName << " Top 5 ] ---\n";
                
                auto sortedUsers = users;
                std::sort(sortedUsers.begin(), sortedUsers.end(), [&gameName](const UserStats& a, const UserStats& b) {
                    int a_val = (a.games.find(gameName) != a.games.end()) ? a.games.at(gameName).value : -1;
                    int b_val = (b.games.find(gameName) != b.games.end()) ? b.games.at(gameName).value : -1;
                    return a_val > b_val;
                });

                int rank = 1;
                for (const auto& u : sortedUsers) {
                    auto it = u.games.find(gameName);
                    if (it != u.games.end() && it->second.value > 0) {
                        if (rank > 5) break;
                        if (it->second.type == "wins") {
                            std::printf(" #%d. %-15s : %d Clears (Played %d times)\n", rank++, u.username.c_str(), it->second.value, it->second.plays);
                        } else {
                            std::printf(" #%d. %-15s : %d pts (Played %d times)\n", rank++, u.username.c_str(), it->second.value, it->second.plays);
                        }
                    }
                }
                if (rank == 1) std::cout << " No records yet.\n";
                std::cout << "\n";
            }
            std::cout << "=========================================\n";
            
            std::cout << "Navigation: ";
            if (totalPages > 1) {
                std::cout << "[N] Next Page | [P] Prev Page | ";
            }
            std::cout << "[E] Exit Leaderboard\n";
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
                    leaderboardPage = (leaderboardPage + 1) % totalPages; 
                    continue; 
                }
                if (cmd == "P") { 
                    leaderboardPage = (leaderboardPage - 1 + totalPages) % totalPages; 
                    continue; 
                }
            }
        }
    }
}