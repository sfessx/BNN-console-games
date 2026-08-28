#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <conio.h>
#include <windows.h>

typedef void (*GetHashFunc)(const char*, char*);
typedef void (*DeleteAccountDataFunc)(const char*);

std::string getHiddenPasswordInternal() {
    std::string password = ""; char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') { if (!password.empty()) { password.pop_back(); std::cout << "\b \b"; } }
        else if (ch != 0 && ch != -32) { password.push_back(ch); std::cout << '*'; }
    }
    std::cout << "\n"; return password;
}

extern "C" {
    __declspec(dllexport) bool isValidUser(const char* username) {
        if (!username || std::strlen(username) == 0) return false;
        for (int i = 0; username[i] != '\0'; i++) {
            char c = username[i];
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')) return false;
        }
        return true;
    }

    __declspec(dllexport) void getHash(const char* password, char* result) {
        if (!password || !result) return;
        unsigned int hash = 5381;
        for (int i = 0; password[i] != '\0'; i++) hash = ((hash << 5) + hash) + password[i];
        std::sprintf(result, "HS_%08X", hash);
    }

    __declspec(dllexport) void registerUser(const char* username) {
        std::cout << "Set your password: "; std::string new_pwd = getHiddenPasswordInternal();
        if (new_pwd.empty()) { std::system("cls"); std::cout << "Password cannot be empty.\n"; return; }
        std::cout << "Confirm your password: "; std::string confirm_pwd = getHiddenPasswordInternal();
        if (new_pwd != confirm_pwd) { std::system("cls"); std::cout << "Error: Passwords do not match!\n"; return; }
        
        char hashed[100] = {0}; getHash(new_pwd.c_str(), hashed);
        std::ofstream outfile("users.txt", std::ios::app);
        outfile << username << "\t" << hashed << "\n"; outfile.close();
        std::system("cls"); std::cout << "Registration successful!\n";
    }

    __declspec(dllexport) void changePassword() {
        std::cout << "Enter username: "; std::string cp_user; std::getline(std::cin, cp_user);
        std::cout << "Enter current password: "; std::string cp_old_pwd = getHiddenPasswordInternal();
        char old_hash[100] = {0}; getHash(cp_old_pwd.c_str(), old_hash);

        std::vector<std::pair<std::string, std::string>> user_list; bool auth_ok = false;
        std::ifstream infile("users.txt"); std::string u_file, p_file;
        while (infile >> u_file >> p_file) {
            if (u_file == cp_user && p_file == std::string(old_hash)) auth_ok = true;
            else user_list.push_back({u_file, p_file});
        }
        infile.close();
        if (!auth_ok) { std::system("cls"); std::cout << "Error: Authentication failed.\n"; return; }

        std::cout << "Set new password: "; std::string cp_new = getHiddenPasswordInternal();
        std::cout << "Confirm new password: "; std::string cp_confirm = getHiddenPasswordInternal();
        if (cp_new.empty() || cp_new != cp_confirm) { std::system("cls"); std::cout << "Error: Invalid passwords!\n"; return; }

        char new_hash[100] = {0}; getHash(cp_new.c_str(), new_hash);
        std::ofstream outfile("users.txt");
        for (auto& u : user_list) outfile << u.first << "\t" << u.second << "\n";
        outfile << cp_user << "\t" << new_hash << "\n"; outfile.close();
        std::system("cls"); std::cout << "Password changed successfully!\n";
    }

    __declspec(dllexport) bool deleteAccountAuth(const char* username) {
        std::cout << "To confirm deletion, please enter your password: ";
        std::string del_pwd = getHiddenPasswordInternal();
        char del_hash[100] = {0}; getHash(del_pwd.c_str(), del_hash);

        bool match = false; std::ifstream verify_file("users.txt"); std::string u_file, p_file;
        while (verify_file >> u_file >> p_file) { if (u_file == username && p_file == std::string(del_hash)) { match = true; break; } }
        verify_file.close();
        if (!match) { std::cout << "Error: Password incorrect.\n"; return false; }

        std::vector<std::pair<std::string, std::string>> remain_users; std::ifstream infile("users.txt");
        while (infile >> u_file >> p_file) { if (u_file != username) remain_users.push_back({u_file, p_file}); }
        infile.close();
        std::ofstream outfile("users.txt"); for (const auto& user : remain_users) outfile << user.first << "\t" << user.second << "\n"; outfile.close();

        HMODULE hStatsDll = LoadLibrary("libs\\stats.dll");
        if (hStatsDll) {
            DeleteAccountDataFunc delData = (DeleteAccountDataFunc)GetProcAddress(hStatsDll, "deleteAccountData");
            if (delData) delData(username);
            FreeLibrary(hStatsDll);
        }
        return true;
    }
}