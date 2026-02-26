//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | eight_bit_int.cpp                                           |
//| EntryPoint   | main                                                        |
//| Purpose      | illustrate how 8 bit integers are represented in binary     |
//| Inputs       | hard coded                                                  |
//| Outputs      | print to screen                                             |
//| Dependencies | none                                                        |
//| By Name,Date | T.Sciple, 02/25/2026                                        |


#include <iostream>
#include <cstdint>
#include <bitset>
#include <iomanip>
#include <cmath>
#include <string>

int main()
{
    // Print the binary representation of all 8-bit integers from -128 to 127
    // In C++, the range of int8_t is from -128 to 127. When we cast an integer to int8_t, it will wrap around if it exceeds this range.
    
    // 1 000  0000  The leftmost bit is the sign bit, which indicates whether the number is positive (0) or negative (1).
    // The remaining 7 bits represent the magnitude of the number.

    //for (int n = -128; n <= 127; ++n) {
    //    std::cout   
    //                << std::setw(5) << std::left << static_cast<int>(static_cast<int8_t>(n)) 
    //                << "binary: " << std::bitset<8>(static_cast<int8_t>(n)) << std::endl;
    //}
    
    // illustrate how to convert 8 bit binary to decimal
    std::cout << "\nConverting 8-bit binary to decimal:\n";
    std::string binaryStr = "10111"; // This represents -128 in 8-bit two's complement

    int len = binaryStr.length(); // Get the index of the last character
    

    for (int i = 0; i < len; i++) {
        std::cout << "(" << binaryStr[i] << " x 2^" << (len - 1 - i) << ")";
        if (i < len - 1) std::cout << " + ";
    }
    std::cout << "\n";
    int n;
    int sum = 0;

    for (int i = 0; i < len; i++) {
        n = binaryStr[i] - '0';
        int exponent = len - 1 - i; // Calculate the exponent based on the position of the bit
        int cur_num = n * pow(2, exponent);
        std::cout << "(" << cur_num << ")";
        if (i < len - 1) std::cout << " + ";
        sum += cur_num; // Convert char to int and calculate value
    }
    std::cout << " = " << sum << std::endl;


    return 0;
}
