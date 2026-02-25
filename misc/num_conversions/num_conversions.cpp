//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | num_conversions.cpp                                         |
//| EntryPoint   | main                                                        |
//| Purpose      | illustrate how to convert numbers between different formats |
//| Inputs       | hard coded                                                  |
//| Outputs      | print to screen                                             |
//| Dependencies | none                                                        |
//| By Name,Date | T.Sciple, 02/23/2026                                        |


#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <bitset>


// Function prototypes
std::string convertNumToBinaryStr(u_int32_t num);
void convertBinaryStrToDecimal(const std::string& bin_str);
std::string FormatWithCommas(u_int32_t num);
void binaryToHexBitset(const std::string& binaryString);


int main() {

    // convert a number to binary string representation
    u_int32_t num = 4'294'967'295; // Max value for uint32_t  = 4'294'967'295
    std::cout << "Step 1: Convert a number to binary string representation\n";
    std::string bin_str = convertNumToBinaryStr(num);
    binaryToHexBitset(bin_str);

    std::cout << "\nStep 2: Convert another number to binary string representation\n";
    num = 23;
    bin_str = convertNumToBinaryStr(num);
    binaryToHexBitset(bin_str);
    
    // convert a binary string to a decimal number, then hexadecimal using bitset
    bin_str = "11111111 11111111 11111111 11111111";
    std::cout << "\nStep 3: Convert a binary string to a decimal number\n";
    convertBinaryStrToDecimal(bin_str);
    binaryToHexBitset(bin_str);
    
    // convert a binary string to a decimal number, then hexadecimal using bitset
    std::cout << "\nStep 4: Convert another binary string to a decimal number\n";
    bin_str = "00000000 00000000 00000000 00010111";
    convertBinaryStrToDecimal(bin_str);
    binaryToHexBitset(bin_str);

    // convert a hex string to a decimal number, then hexadecimal using bitset
    std::cout << "\nStep 5: Convert a hex string to a decimal number\n";
    std::string hex_str = "1F";
    std::cout << "\tHexStr: " << hex_str << std::endl;
    u_int32_t decimal_num = std::stoul(hex_str, nullptr, 16);
    std::cout << "\tDecimalNum: " << FormatWithCommas(decimal_num) << std::endl;
    bin_str = convertNumToBinaryStr(decimal_num);
    binaryToHexBitset(bin_str);

    return 0;
}


void convertBinaryStrToDecimal(const std::string& bin_str) {
    // Remove spaces from the binary string
    std::string cleaned_bin_str;
    for (char c : bin_str) {
        if (c != ' ') {
            cleaned_bin_str += c;
        }
    }

    // Convert binary string to decimal number
    u_int32_t decimal_num = 0;
    int power = 0;
    for (auto it = cleaned_bin_str.rbegin(); it != cleaned_bin_str.rend(); ++it) {
        if (*it == '1') {
            decimal_num += (1 << power); // 2^power
        }
        power++;
    }
    std::cout   << "\tBinaryStr: " << bin_str << "\n"
                << "\tDecimalNum: " << FormatWithCommas(decimal_num) << std::endl;
}


std::string convertNumToBinaryStr(u_int32_t num) {
    u_int32_t n = num;

    // Mannually Convert integer to binary string representation
    std::string binaryStr;
    int iter = 0;
    while (n > 0 || iter < 32) {
        if (iter % 8 == 0 && iter != 0) {
            binaryStr += " ";
        }
        binaryStr += std::to_string(n % 2);
        // std::cout   << std::setw(12) << std::left << "iter: " + std::to_string(iter)
        //             << std::setw(16) << std::left << "n: " + std::to_string(n)
        //             << std::setw(10) << std::left << "bit: " + std::to_string(n % 2) << std::endl;
         n = n / 2;
         iter++;
    }

    // Reverse the binary string

    std::reverse(binaryStr.begin(), binaryStr.end());

    std::cout   << "\tNow Reverse the bits to form the binary - little endian format with least significant bit first\n"
                << "\tnum: " << FormatWithCommas(num) << "\n"
                << "\tBinaryStr: " << binaryStr << std::endl;
    
    return binaryStr;
}


void binaryToHexBitset(const std::string& binaryString) {
    std::string cleanedBinary;
    cleanedBinary.reserve(binaryString.size());

    for (char c : binaryString) {
        if (c == ' ') {
            continue;
        }
        if (c != '0' && c != '1') {
            std::cerr << "Invalid binary string: only 0, 1, and spaces are allowed.\n";
            return;
        }
        cleanedBinary += c;
    }

    if (cleanedBinary.size() != 32) {
        std::cerr << "Invalid binary string length: expected 32 bits, got "
                  << cleanedBinary.size() << ".\n";
        return;
    }

    u_int32_t decimalValue = static_cast<u_int32_t>(std::bitset<32>(cleanedBinary).to_ulong());

    std::cout << "\tThe hexadecimal equivalent is: 0x"
              << std::hex << std::uppercase << decimalValue << std::dec << std::endl;
}


std::string FormatWithCommas(u_int32_t num) {
    std::string str = std::to_string(num);
    int bkw_cnt = str.length(); // initialize a backward counter to count down from string len to 0
    std::string formated_str;

    for (char c : str) {     // loops thru each character in the string
        formated_str += c;
        bkw_cnt--;  // deincrement the count by 1 after the first character which will always be there before checking for needed commas
        if (bkw_cnt != 0) {
            if(bkw_cnt % 3 == 0) {  // checks if the mod of bkw_cnt / 3 == 0, or remainder is 0
                    formated_str += ',';
            }
        }
    }
    return formated_str;
}

