#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

std::string pad_lead_zeros(int number, int no_digits) {
    std::ostringstream oss;
    oss << std::setw(no_digits) << std::setfill('0') << number;
    return oss.str();
}

int main() {
    int numbers[] = {8, 1, 123, 4567};

    for (int num : numbers) {
        std::string padded = pad_lead_zeros(num, 6);
        std::cout << std::left << std::setw(8) << std::to_string(num) + " -> " << padded << std::endl;
    }

    return 0;
}
