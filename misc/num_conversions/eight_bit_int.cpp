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

int main()
{
    // Print the binary representation of all 8-bit integers from -128 to 127
    // In C++, the range of int8_t is from -128 to 127. When we cast an integer to int8_t, it will wrap around if it exceeds this range.
    
    // 1 000  0000  The leftmost bit is the sign bit, which indicates whether the number is positive (0) or negative (1).
    // The remaining 7 bits represent the magnitude of the number.

    for (int n = -128; n <= 127; ++n) {
        std::cout   
                    << std::setw(5) << std::left << static_cast<int>(static_cast<int8_t>(n)) 
                    << "binary: " << std::bitset<8>(static_cast<int8_t>(n)) << std::endl;
    }

    return 0;
}
