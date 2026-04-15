#pragma once

#include <string>

class InputReader {
public:
    static int getIntInRange(const std::string& prompt, int min_value, int max_value);
    static double getDouble(const std::string& prompt, double default_value, bool allow_default);

private:
    static std::string getLine();
};
