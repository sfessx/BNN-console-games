#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <conio.h>
#include <vector>
#include <windows.h>
#include <ctime>
#include <algorithm>

using namespace std;

typedef bool (*IsValidUserFunc)(const char*);
typedef void (*GetHashFunc)(const char*, char*);
typedef void (*RecordLoginFunc)(const char*);
typedef void (*GetStatsFunc)(const char*, char*);
typedef void (*VoidDllFunc)(); 
typedef bool (*DeleteAuthFunc)(const char*);
typedef const char* (*GetGameNameFunc)();
typedef void (*PlayGameFunc)(const char*);
typedef const char* (*GetLibMenuNameFunc)();
typedef void (*ExecFunc)(const char*);
typedef void (*RegisterFunc)(const char*);
typedef void (*ProfileFunc)(const char*, int);

IsValidUserFunc isValidUser; GetHashFunc myGetHash; RecordLoginFunc recordLogin; GetStatsFunc getStats;

struct PluginItem { string filename; string menuName; };

string getHiddenPassword() {
    string password = ""; char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') { if (!password.empty()) { password.pop_back(); cout << "\b \b"; } }
        else if (ch != 0 && ch != -32) { password.push_back(ch); cout << '*'; }
    }
    cout << "\n"; return password;
}

vector<PluginItem> scanDir(const string& folder, const string& filter, bool isGame) {
    vector<PluginItem> plugins; WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile((folder + "\\" + filter).c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            string filename = findData.cFileName; string fullPath = folder + "\\" + filename;
            HMODULE hMod = LoadLibrary(fullPath.c_str());
            if (hMod) {
                if (isGame) {
                    GetGameNameFunc f = (GetGameNameFunc)GetProcAddress(hMod, "getGameName");
                    if (f) plugins.push_back({filename, f()});
                } else {
                    GetLibMenuNameFunc f = (GetLibMenuNameFunc)GetProcAddress(hMod, "getLibMenuName");
                    if (f) plugins.push_back({filename, f()});
                }
                FreeLibrary(hMod);
            }
        } while (FindNextFile(hFind, &findData));
        FindClose(hFind);
    }
    sort(plugins.begin(), plugins.end(), [](const PluginItem& a, const PluginItem& b) { return a.menuName < b.menuName; });
    return plugins;
}

int main() {
    std::time_t sessionStartTime = std::time(0);
    HMODULE hDll = LoadLibrary("libs\\security.dll");
    HMODULE hStatsDllReal = LoadLibrary("libs\\stats.dll");
    
    if (!hDll || !hStatsDllReal) { cout << "Error: Missing system libraries!\n"; cin.get(); return 1; }
    
    isValidUser = (IsValidUserFunc)GetProcAddress(hDll, "isValidUser"); myGetHash = (GetHashFunc)GetProcAddress(hDll, "getHash");
    recordLogin = (RecordLoginFunc)GetProcAddress(hStatsDllReal, "recordLogin"); getStats = (GetStatsFunc)GetProcAddress(hStatsDllReal, "getStats");
    if (!isValidUser || !myGetHash || !recordLogin || !getStats) { cout << "Error: Libraries are broken!\n"; cin.get(); return 1; }

    string s, ans, u_file, p_file;
    while (true) {
        cout << "Please type a user name(Guest / cp / username): "; getline(cin, s);
        if (s == "Guest") break;
        if (s == "cp") {
            VoidDllFunc changePassword = (VoidDllFunc)GetProcAddress(hDll, "changePassword");
            if (changePassword) changePassword(); continue;
        }
        if (!isValidUser(s.c_str())) { system("cls"); cout << "Error: Only letters, numbers, and underscores allowed.\n"; continue; }
        
        bool exists = false; ifstream infile("users.txt");
        while (infile >> u_file >> p_file) { if (u_file == s) { exists = true; break; } }
        infile.close();
        if (exists) {
            cout << "Enter password: "; string input_pwd = getHiddenPassword();
            char hashed[100] = {0}; myGetHash(input_pwd.c_str(), hashed);
            if (string(hashed) == p_file) { cout << "Login successful!\n"; break; }
            system("cls"); cout << "Wrong password! Try again.\n"; continue;
        }
        while (true) { cout << "Username not found. Register?[Y/N]: "; getline(cin, ans); if (ans == "Y" || ans == "y" || ans == "N" || ans == "n") break; }
        if (ans == "N" || ans == "n") { system("cls"); continue; }
        
        RegisterFunc registerUser = (RegisterFunc)GetProcAddress(hDll, "registerUser");
        if (registerUser) { registerUser(s.c_str()); break; }
    }
    
    recordLogin(s.c_str()); sessionStartTime = std::time(0);
    
    int gameCurrentPage = 0;
    const int GAMES_PER_PAGE = 9;

    while (true) {
        system("cls"); cout << "User: " << s << "\n\n";
        
        vector<PluginItem> gamesList = scanDir("games", "*.dll", true);
        vector<PluginItem> libsList = scanDir("libs", "lib_*.dll", false);
        
        int totalGames = gamesList.size();
        int maxPages = (totalGames + GAMES_PER_PAGE - 1) / GAMES_PER_PAGE;
        if (gameCurrentPage >= maxPages && maxPages > 0) gameCurrentPage = maxPages - 1;

        int startIndex = gameCurrentPage * GAMES_PER_PAGE;
        int endIndex = min(startIndex + GAMES_PER_PAGE, totalGames);

        cout << "--- [ Games Page " << gameCurrentPage + 1 << " / " << (maxPages == 0 ? 1 : maxPages) << " ] ---\n";
        for (int i = startIndex; i < endIndex; i++) {
            int localIdx = i - startIndex + 1;
            cout << " " << localIdx << ". Play " << gamesList[i].menuName << "\n";
        }
        cout << "\n";

        cout << " [M] View User Profile\n";
        cout << " [B] View System Leaderboard\n";
        cout << " [D] Delete Account\n";
        cout << " [E] Logout & Exit\n\n";
        
        if (maxPages > 1) {
            cout << "Navigation: [N] Next Page | [P] Prev Page\n";
        }
        cout << "Select (1-9) or Key (M/B/D/E): ";
        
        string choiceStr; getline(cin, choiceStr);
        if (choiceStr.empty()) continue;

        string cmd = choiceStr;
        transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

        if (maxPages > 1) {
            if (cmd == "N") { gameCurrentPage = (gameCurrentPage + 1) % maxPages; continue; }
            if (cmd == "P") { gameCurrentPage = (gameCurrentPage - 1 + maxPages) % maxPages; continue; }
        }

        if (cmd == "M") {
            ProfileFunc showProfile = (ProfileFunc)GetProcAddress(hStatsDllReal, "showProfile");
            if (showProfile) {
                std::time_t curr = std::time(0);
                showProfile(s.c_str(), static_cast<int>(curr - sessionStartTime));
                sessionStartTime = std::time(0);
            }
            continue;
        }
        else if (cmd == "B") {
            if (!libsList.empty()) {
                string fullPath = "libs\\" + libsList[0].filename;
                HMODULE hLib = LoadLibrary(fullPath.c_str());
                if (hLib) {
                    ExecFunc executeFunction = (ExecFunc)GetProcAddress(hLib, "executeFunction");
                    if (executeFunction) executeFunction(s.c_str());
                    FreeLibrary(hLib);
                }
            } else {
                system("cls"); cout << "Error: Leaderboard plugin missing!\n"; cin.get();
            }
            continue;
        }
        else if (cmd == "D") {
            system("cls"); if (s == "Guest") { cout << "Error: Guest account cannot be deleted.\n\nPress Enter to return..."; cin.get(); continue; }
            DeleteAuthFunc deleteAccountAuth = (DeleteAuthFunc)GetProcAddress(hDll, "deleteAccountAuth");
            if (deleteAccountAuth && deleteAccountAuth(s.c_str())) {
                cout << "Account deleted successfully!\nPress Enter to exit..."; cin.get(); break;
            }
            cout << "\nPress Enter to return..."; cin.get();
            continue;
        }
        else if (cmd == "E") {
            if (s != "Guest") {
                std::time_t curr = std::time(0); char stats_res[64] = {0};
                std::sprintf(stats_res, "%d", static_cast<int>(curr - sessionStartTime)); getStats(s.c_str(), stats_res);
            }
            cout << "Goodbye.\n"; break;
        }

        int choice = atoi(cmd.c_str());
        int currentPageItemsCount = endIndex - startIndex;
        
        if (choice >= 1 && choice <= currentPageItemsCount) {
            int realGameIndex = startIndex + (choice - 1);
            string fullPath = "games\\" + gamesList[realGameIndex].filename;
            HMODULE hGame = LoadLibrary(fullPath.c_str());
            if (hGame) {
                PlayGameFunc playGame = (PlayGameFunc)GetProcAddress(hGame, "playGame");
                if (playGame) playGame(s.c_str());
                FreeLibrary(hGame);
            }
        }
    }
    FreeLibrary(hDll); FreeLibrary(hStatsDllReal); return 0;
}