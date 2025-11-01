#include <iostream>
#include <string>
#include <sstream>
#include <bitset>
#include <iomanip>

//Function Prototypes
std::string getUserInput();
void convertBinaryStrToAscii(std::string& binaryString);
void convertEightBitBinaryStrToHex(std::string& binaryString);
void convertEightBitBinaryStrToDec(std::string& binaryString);

int main() {
    std::string str;
    str = getUserInput();
    convertBinaryStrToAscii(str);
    convertEightBitBinaryStrToHex(str);
    convertEightBitBinaryStrToDec(str);
    std::cout << "\n";
    system("pause");
    return 0;
}

std::string getUserInput() {
    std::string str;      //Example = "0100111101101110011000110110010100100000011101010111000001101111011011100010000001100001001000000111010001101001011011010110010100100000011010010110111000100000011000010010000001110011011011010110000101101100011011000010000001110110011010010110110001101100011000010110011101100101001011000010000001110100011010000110010101110010011001010010000001101100011010010111011001100101011001000010000001100001001000000111011101101001011100110110010100100000011011110110110001100100001000000110110101100001011011100010111000100000010010000110010100100000011101110110000101110011001000000110101101101110011011110111011101101110001000000110011001100001011100100010000001100001011011100110010000100000011101110110100101100100011001010010000001100110011011110111001000100000011010000110100101110011001000000110101101101110011011110111011101101100011001010110010001100111011001010010000001100001011011100110010000100000011010110110100101101110011001000110111001100101011100110111001100101110"
    std::cout << "Enter Binary Digits to convert to ascii in increments of 8 digits representing a byte \n Enter Here --> ";
    std::getline(std::cin, str);
    return str;
}

void convertBinaryStrToAscii(std::string& binaryString) {
    std::cout << "\nConverted Binary string to ASCII:\n";
    for(int i = 0; i < binaryString.length(); i += 8) {
        std::string s;  
        s = binaryString.substr(i,8);    // get 8 characters representing each byte
        std::bitset<8> bitSetVar(s);  // convert the string to a bitset
        std::cout << static_cast<char>(bitSetVar.to_ulong());  // cast the bitSetVar to Long and then charcter
    }
    std::cout << std::endl;  // add a new line at the end of output
}

void convertEightBitBinaryStrToHex(std::string& binaryString) {
    std::cout << "\nConverted Binary string to Hexidecimal:\n";
    for(int i = 0; i < binaryString.length(); i += 8) {
        std::string s;  
        s = binaryString.substr(i,8);    // get 8 characters representing each byte
        std::bitset<8> bitSetVar(s);  // convert the string to a bitset
        long num = bitSetVar.to_ulong();
        std::cout << std::hex << std::uppercase << num << " " ;  // print the number in hex format
    }
    std::cout << std::endl;  // add a new line at the end of output
}

void convertEightBitBinaryStrToDec(std::string& binaryString) {
    std::cout << "\nConverted Binary string to ASCII Decimal Equivalents:\n";
    for(int i = 0; i < binaryString.length(); i += 8) {
        std::string s;  
        s = binaryString.substr(i,8);    // get 8 characters representing each byte
        std::bitset<8> bitSetVar(s);  // convert the string to a bitset
        long num = bitSetVar.to_ulong();
        std::cout << std::setfill('0') << std::setw(3) << std::dec << num << " " ;  // print the number in hex format
    }
    std::cout << std::endl;  // add a new line at the end of output
}
