#pragma once

#include <string>
#include <vector>

class ConsoleMenu {
public:
    static void clearScreen();
    static void printHeader(const std::string& title);
    static int promptMenu(const std::string& title, const std::vector<std::string>& options, int min_option, int max_option);
    static void pause();
};
