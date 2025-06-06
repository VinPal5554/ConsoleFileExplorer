#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <windows.h>

#ifdef _WIN32
#include <windows.h>  // For getting user folder on Windows
#endif

const std::string COLOR_RESET = "\033[0m";
const std::string COLOR_BLUE = "\033[34m";
const std::string COLOR_GREEN = "\033[32m";
const std::string COLOR_GRAY = "\033[90m";

namespace fs = std::filesystem;

fs::path getStartingDirectory() {
    fs::path startPath;

    // Check the OS and set the starting directory accordingly
#ifdef _WIN32
    // For Windows: Start at Desktop or User directory
    char* userProfile = nullptr;
    size_t len = 0;
    if (_dupenv_s(&userProfile, &len, "USERPROFILE") == 0 && userProfile != nullptr) {
        startPath = fs::path(userProfile) / "Desktop";  // Default to Desktop
        free(userProfile); // Free the memory allocated by _dupenv_s
    }
#else
    // For Linux/macOS: Start at home directory
    const char* homeDir = getenv("HOME");
    if (homeDir) {
        startPath = fs::path(homeDir);  // Default to the user's home directory
    }
#endif

    // If path isn't found, fallback to current working directory
    if (startPath.empty() || !fs::exists(startPath)) {
        startPath = fs::current_path();
    }

    return startPath;
}

void listDirectory(const fs::path& path) {
    std::cout << "\nContents of: " << path << "\n";

    for (const auto& entry : fs::directory_iterator(path)) {
        const auto filename = entry.path().filename().string();
        std::string color = COLOR_RESET;

        if (fs::is_directory(entry)) {
            color = COLOR_BLUE;
        }
        else if (entry.path().extension() == ".exe" || entry.path().extension() == ".sh") {
            color = COLOR_GREEN;
        }
        else if (filename[0] == '.') { // Hidden file on Unix/macOS
            color = COLOR_GRAY;
        }

        std::cout << (fs::is_directory(entry) ? "[DIR]  " : "       ");
        std::cout << color << filename << COLOR_RESET << "\n";
    }
}

std::string getBreadcrumbPrompt(const fs::path& path) {
    std::string prompt = "[";
    bool first = true;
    for (const auto& part : path) {
        if (!first) prompt += " > ";
        prompt += part.string();
        first = false;
    }
    prompt += "] > ";
    return prompt;
}


int main() {
#ifdef _WIN32
    // Enable ANSI escape sequences on Windows 10+ terminals
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (hOut != INVALID_HANDLE_VALUE &&
        GetConsoleMode(hOut, &dwMode)) {
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif
    fs::path currentPath = getStartingDirectory();  // Start from a sensible location
    std::string command;

    while (true) {
        listDirectory(currentPath);
        std::cout << "\nCommands: cd <folder>, up, open <file>, quit\n";
        std::cout << getBreadcrumbPrompt(currentPath);

        std::getline(std::cin, command);

        if (command == "quit") break;
        else if (command == "up") {
            if (currentPath.has_parent_path())
                currentPath = currentPath.parent_path();
        }
        else if (command.rfind("cd ", 0) == 0) {
            std::string folder = command.substr(3);
            fs::path newPath = currentPath / folder;
            if (fs::is_directory(newPath)) currentPath = newPath;
            else std::cout << "Invalid directory.\n";
        }
        else if (command.rfind("open ", 0) == 0) {
            std::string file = command.substr(5);
            fs::path filePath = currentPath / file;
            if (fs::is_regular_file(filePath)) {
                std::ifstream inFile(filePath);
                if (!inFile) {  // Check if file opened successfully
                    std::cout << "Failed to open file.\n";
                    continue;
                }
                std::string line;
                while (std::getline(inFile, line)) {
                    std::cout << line << "\n";
                }
                inFile.close();
            }
            else std::cout << "Not a valid file.\n";
        }
        else {
            std::cout << "Unknown command.\n";
        }
    }

    return 0;
}


