#include "../include/input_reader.hpp"

#include <iostream>

int InputReader::getIntInRange(const std::string& prompt, int min_value, int max_value) {
    while (true) {
        std::cout << prompt;
        const std::string line = getLine();
        try {
            std::size_t parsed = 0;
            const int value = std::stoi(line, &parsed);
            if (parsed == line.size() && value >= min_value && value <= max_value) {
                return value;
            }
        } catch (...) {
        }
        std::cout << "Invalid selection. Enter a number between " << min_value << " and " << max_value << ".\n";
    }
}

double InputReader::getDouble(const std::string& prompt, double default_value, bool allow_default) {
    while (true) {
        std::cout << prompt;
        const std::string line = getLine();
        if (allow_default && line.empty()) {
            return default_value;
        }
        try {
            std::size_t parsed = 0;
            const double value = std::stod(line, &parsed);
            if (parsed == line.size()) {
                return value;
            }
        } catch (...) {
        }
        std::cout << "Invalid numeric input. Please try again.\n";
    }
}

std::string InputReader::getLine() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}
