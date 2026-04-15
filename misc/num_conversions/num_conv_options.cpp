//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | num_conversions.cpp                                         |
//| EntryPoint   | main                                                        |
//| Purpose      | illustrate how to convert numbers between different formats |
//| Inputs       | hard coded                                                  |
//| Outputs      | print to screen                                             |
//| Dependencies | none                                                        |
//| By Name,Date | T.Sciple, 04/14/2026                                        |


#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <bitset>
#include <cstdint>
#include <cctype>


// Function prototypes
void clearScreen();
int inputMenu();
std::string getUserInput();
bool askToConvertAnother();
int processUserInput(int option, const std::string& input);
void convertDecToAllTypes(uint32_t decimal_num);
std::string ConvertDecToBinaryStr(uint32_t num);
uint32_t convertBinaryStrToDecimal(const std::string& bin_str);
std::string FormatWithCommas(uint32_t num);
std::string binaryToHexBitset(const std::string& binaryString);


enum {
    DecInput = 1,
    BinInput,
    HexInput,
    Exit
};


int main() {

    // Main Menu

    // Create a loop to display the menu and process user input until they choose to exit
    do
    {
        clearScreen();
        int option = inputMenu();
        if (option == Exit) {
            std::cout << "Exiting the program.\n";
            return 0;
        } else if (option < DecInput || option > HexInput) {
            std::cerr << "Invalid option selected. Please try again.\n";
            continue; // Prompt the user again
        } else {
            std::string input = getUserInput();
            if (processUserInput(option, input) == -1) {
                continue; // Invalid input, prompt the user again
            }
            if (!askToConvertAnother()) {
                std::cout << "Exiting the program.\n";
                break;
            }
        }
    } while (true);

    return 0;
}


void clearScreen() {
    // Clear the console screen (platform-specific)
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


std::string getUserInput() {

    std::cout << "\nUser Input Number: ";
    std::string input;
    // read the entire line of input, including spaces, into the string variable 'input'
    std::cin.ignore(); // ignore any leftover newline character in the input buffer
    std::getline(std::cin, input);

    std::transform(input.begin(), input.end(), input.begin(), ::toupper); // convert input to uppercase for consistency
    return input;
}


bool askToConvertAnother() {
    std::string answer;

    while (true) {
        std::cout << "\nDo you want to do another conversion? (y/n): ";
        std::getline(std::cin >> std::ws, answer);

        if (answer.empty()) {
            continue;
        }

        char choice = static_cast<char>(std::tolower(static_cast<unsigned char>(answer[0])));
        if (choice == 'y') {
            return true;
        }
        if (choice == 'n') {
            return false;
        }

        std::cout << "Please enter y or n.\n";
    }
}


int processUserInput(int option, const std::string& input) {
    uint32_t decimal_num = 0;

    try {
        switch (option) {
            case DecInput:
                decimal_num = static_cast<uint32_t>(std::stoul(input, nullptr, 10));
                break;
            case BinInput:
                decimal_num = convertBinaryStrToDecimal(input);
                break;
            case HexInput:
                decimal_num = static_cast<uint32_t>(std::stoul(input, nullptr, 16));
                break;
            default:
                std::cerr << "Invalid option selected. Please try again.\n";
                return -1; // Indicate an error
        }
    } catch (const std::exception& ex) {
        std::cerr << "Invalid input: " << ex.what() << "\n";
        return -1; // Indicate an error
    }

    // Any input is converted to decimal and then passed to the function to convert to all types for output
    convertDecToAllTypes(decimal_num);
    return 0; // Indicate success
}


int inputMenu() {
    std::cout << "Number Conversion Options:\n\n"
              << "   | Code |  Input  |\n"
              << "   |------|---------|\n"
              << "   |   1  | Decimal |\n"
              << "   |   2  | Binary  |\n"
              << "   |   3  | Hex     |\n"
              << "   |   4  | Exit    |\n\n"
              << "    Enter the code for the desired conversion: ";
    
    int option;
    std::cin >> option;

    return option;
}


void convertDecToAllTypes(uint32_t decimal_num) {

    std::string bin_str = ConvertDecToBinaryStr(decimal_num);

    //clear screen for output results
    std::cout << "\nResults:\n";
    std::cout << std::left << std::setw(15) << "\tDecimal: " << FormatWithCommas(decimal_num) << std::endl;
    std::cout << std::left << std::setw(15) << "\tBinary: " << bin_str << std::endl;
    std::cout << std::left << std::setw(15) << "\tHex: " << std::hex << std::uppercase << decimal_num << std::dec << std::endl;
}


uint32_t convertBinaryStrToDecimal(const std::string& bin_str) {
    // Remove spaces from the binary string
    std::string cleaned_bin_str;
    for (char c : bin_str) {
        if (c == ' ') {
            continue;
        }
        if (c != '0' && c != '1') {
            throw std::invalid_argument("binary input can only include 0, 1, and spaces");
        }
        if (c != ' ') {
            cleaned_bin_str += c;
        }
    }

    if (cleaned_bin_str.empty() || cleaned_bin_str.size() > 32) {
        throw std::invalid_argument("binary input must contain 1 to 32 bits");
    }

    // Convert binary string to decimal number
    uint32_t decimal_num = 0;
    int power = 0;
    for (auto it = cleaned_bin_str.rbegin(); it != cleaned_bin_str.rend(); ++it) {
        if (*it == '1') {
            decimal_num += (1u << power); // 2^power
        }
        power++;
    }
    return decimal_num;
}


std::string ConvertDecToBinaryStr(uint32_t num) {
    uint32_t n = num;

    // Mannually Convert integer to binary string representation
    std::string binaryStr;
    int iter = 0;
    while (n > 0 || iter < 32) {
        if (iter % 8 == 0 && iter != 0) {
            binaryStr += " ";
        }
        binaryStr += std::to_string(n % 2);
        n = n / 2;
        iter++;
    }

    // Reverse the binary string
    std::reverse(binaryStr.begin(), binaryStr.end());

    return binaryStr;
}


std::string  binaryToHexBitset(const std::string& binaryString) {
    std::string cleanedBinary;
    cleanedBinary.reserve(binaryString.size());

    for (char c : binaryString) {
        if (c == ' ') {
            continue;
        }
        if (c != '0' && c != '1') {
            std::cerr << "Invalid binary string: only 0, 1, and spaces are allowed.\n";
            return "Error";
        }
        cleanedBinary += c;
    }

    if (cleanedBinary.size() != 32) {
        std::cerr << "Invalid binary string length: expected 32 bits, got "
                  << cleanedBinary.size() << ".\n";
        return "Error";
    }

    uint32_t decimalValue = static_cast<uint32_t>(std::bitset<32>(cleanedBinary).to_ulong());
    std::string hexString = std::to_string(decimalValue);

    return hexString;
}


std::string FormatWithCommas(uint32_t num) {
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