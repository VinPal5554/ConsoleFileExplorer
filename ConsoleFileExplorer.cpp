#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>

namespace fs = std::filesystem;

void listDirectory(const fs::path& path) {
    std::cout << "\nContents of: " << path << "\n";
    for (const auto& entry : fs::directory_iterator(path)) {
        std::cout << (fs::is_directory(entry) ? "[DIR]  " : "       ");
        std::cout << entry.path().filename().string() << "\n";
    }
}

int main()
{
    fs::path currentPath = fs::current_path();
    std::string command;

    while (true) {
        listDirectory(currentPath);
        std::cout << "\nCommands: cd <folder>, up, open <file>, quit\n> ";
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
                std::string line;
                while (std::getline(inFile, line))
                    std::cout << line << "\n";
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


