#include <string>
#include <iostream>


template<typename Integral>
std::string FormatWithCommas(Integral value) {
    std::string s = std::to_string(std::abs(value));
    std::string result;
    if (value < 0) result += '-';

    int len = s.length();
    for (int i = 0; i < len; ++i) {
        if (i > 0 && (len - i) % 3 == 0) {
            result += ',';
        }
        result += s[i];
    }
    return result;
}


int main(int argc, char const *argv[])
{
    int num1 = 1234567;
    long long int num2 = 1234567890123;

    std::cout << "Formatted int: " << FormatWithCommas(num1) << "\n";
    std::cout << "Formatted long long int: " << FormatWithCommas(num2) << "\n";

    return 0;
}
