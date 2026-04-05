#include "../include/number_formatter.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>

std::string NumberFormatter::formatDoubleWithCommas(double value, int max_significant_digits, const std::string& label) {
    if (max_significant_digits < 1) {
        max_significant_digits = 1;
    }

    double abs_value = std::fabs(value);
    int decimals = 0;

    if (abs_value > 0.0) {
        double exponent = std::floor(std::log10(abs_value));
        decimals = max_significant_digits - 1 - static_cast<int>(exponent);
        if (decimals < 0) {
            decimals = 0;
        }
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(decimals) << value;
    std::string text = oss.str();

    std::size_t dot_pos = text.find('.');
    std::size_t int_end = (dot_pos == std::string::npos) ? text.size() : dot_pos;
    std::size_t comma_start = (text[0] == '-') ? 1 : 0;

    for (std::size_t i = int_end; i > comma_start + 3; i -= 3) {
        text.insert(i - 3, ",");
    }

    if (dot_pos != std::string::npos) {
        while (!text.empty() && text.back() == '0') {
            text.pop_back();
        }
        if (!text.empty() && text.back() == '.') {
            text.pop_back();
        }
    }

    return text + (label.empty() ? "" : " " + label);
}
