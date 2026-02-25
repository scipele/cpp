#include <iostream>
#include <string>
#include <algorithm>

int main()  
{
    int32_t orig_num = 2147483647;
    int32_t n = orig_num;
    // Mannually Convert integer to binary string representation
    std::string binaryStr;
    int iter = 0;
    while (n > 0 || iter < 8) {
        binaryStr += std::to_string(n % 2);
         n = n / 2;
         iter++;
    }

    // Reverse the binary string
    std::reverse(binaryStr.begin(), binaryStr.end());

    std::cout << "num: " << orig_num << ", BinaryStr: " << binaryStr << std::endl;

    return 0;
}
