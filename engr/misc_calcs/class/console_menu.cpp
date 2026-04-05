#include "../include/console_menu.hpp"
#include "../include/input_reader.hpp"

#include <iostream>

void ConsoleMenu::clearScreen() {
    std::cout << "\033[2J\033[H";
}

void ConsoleMenu::printHeader(const std::string& title) {
    std::cout << title << "\n";
    std::cout << std::string(title.size(), '=') << "\n\n";
}

int ConsoleMenu::promptMenu(const std::string& title, const std::vector<std::string>& options, int min_option, int max_option) {
    clearScreen();
    printHeader(title);
    for (const std::string& option : options) {
        std::cout << option << "\n";
    }
    std::cout << "\n";
    return InputReader::getIntInRange("Selection: ", min_option, max_option);
}

void ConsoleMenu::pause() {
    std::cout << "\nPress Enter to continue...";
    std::string sink;
    std::getline(std::cin, sink);
}
