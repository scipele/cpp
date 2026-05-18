//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | eight_bit_int.cpp                                           |
//| EntryPoint   | main                                                        |
//| Purpose      | illustrate how 8 bit integers are represented in binary     |
//| Inputs       | hard coded                                                  |
//| Outputs      | print to screen                                             |
//| Dependencies | none                                                        |
//| By Name,Date | T.Sciple, 05/18/2026                                        |

#include <iostream>
#include <cstdint>
#include <bitset>
#include <iomanip>
#include <cmath>
#include <string>


void printBinaryToDecimalCalc(std::string binaryStr);
void printBinaryToConversionToHexAndDecimal(std::string binaryStr);


int main()
{
    std::string binaryStr = "01101101"; // This represents 8 in 8-bit two's complement

    printBinaryToDecimalCalc(binaryStr);
    printBinaryToConversionToHexAndDecimal(binaryStr);

    std::cout << "\n" << std::endl;
    return 0;
}


void printBinaryToDecimalCalc(std::string binaryStr) {
    // 1. illustrate how to convert 8 bit binary to decimal
    std::cout   << "\n1. Converting 8-bit binary to decimal:\n"
                << "\tGiven Binary String: " << binaryStr << "\n\t";

    int len = binaryStr.length();   // Get the index of the last character
    std::cout << "   ";             // Initial spacing for alignment

    int num, total = 0;
    for (int i = 0; i < len; i++) {
        num = (binaryStr[i] - '0') * pow(2, len - 1 - i);
        std::cout << "(" << binaryStr[i] << " x 2^" << (len - 1 - i) << ")\t = "
        << std::setw(5) << std::right << num << "\n\t";
        if (i < len - 1) std::cout << " + ";
        total += num;
    }

    std::cout   << "-------------------------\n"
                << "\tTotal            = " << std::setw(5) << std::right << total << "\n\n";
}


void printBinaryToConversionToHexAndDecimal(std::string binaryStr ) {
    // Next convert the high nibble (the leftmost 4 bits) to decimal and the low nibble (the rightmost 4 bits) to decimal, then add them together
    std::cout   << "\n2. Converting 8-bit binary to decimal using nibbles:\n"
                << "\tDesc           Binary          Hex\n";

    std::string highNibble = binaryStr.substr(0, 4); // Get the high nibble (first 4 bits)
    std::string lowNibble = binaryStr.substr(4, 4); // Get the low nibble (last 4 bits)

    std::cout   << "\tHigh nibble:\t" << highNibble << "\t\t" << std::hex << std::stoi(highNibble, nullptr, 2) << "\n"
                << "\tLow nibble:\t"  << lowNibble << "\t\t" << std::hex << std::stoi(lowNibble, nullptr, 2) << "\n"
                << "\tBinary to Hex:\t" << std::hex << std::stoi(binaryStr, nullptr, 2) << "\n";
    std::cout   << std::endl;

    int numa = std::stoi(highNibble, nullptr, 2); // Convert high nibble to decimal
    int numb = std::stoi(lowNibble, nullptr, 2);  // Convert low nibble to decimal
    int total = numa * 16 + numb;                 // Add the two numbers together   
    std::cout << std::dec; // Switch back to decimal output
    std::cout   << "\tDecimal from high nibble: " << numa << " x 16 = " << numa * 16 << "\n"
                << "\tDecimal from low nibble:  " << numb << "\n"
                << "\tTotal (high + low):       " << total << "\n\n";
}