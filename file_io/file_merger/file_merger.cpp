#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    std::ofstream outputFile("combined.txt", std::ios::app);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open combined.txt for writing.\n";
        return 1;
    }

    std::string separator;
    std::cout << "Enter optional text to insert between each file (or leave blank for none): ";
    std::getline(std::cin, separator);

    for (const auto& entry : fs::directory_iterator(".")) {
        if (entry.path().extension() == ".sql") {
            std::ifstream inputFile(entry.path());
            if (!inputFile.is_open()) {
                std::cerr << "Failed to open " << entry.path().filename() << " for reading.\n";
                continue;
            }

            std::cout << "Appending " << entry.path().filename() << "...\n";
            //outputFile << "===== Contents of " << entry.path().filename() << " =====\n";
            
            std::string line;
            while (std::getline(inputFile, line)) {
                outputFile << line << "\n";
            }

            // Optionally add separator text
            if (!separator.empty()) {
                outputFile << separator << "\n";
            }

            inputFile.close();
        }
    }

    outputFile.close();
    std::cout << "All .sql files have been combined into combined.txt.\n";

    return 0;
}
