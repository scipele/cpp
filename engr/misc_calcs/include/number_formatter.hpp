#pragma once

#include <string>

class NumberFormatter {
public:
    static std::string formatDoubleWithCommas(double value, int max_significant_digits = 5, const std::string& label = "");
};
