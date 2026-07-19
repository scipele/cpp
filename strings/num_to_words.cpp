/*
| Item	       | Main Program Documentation Notes                            |
|--------------|-------------------------------------------------------------|
| Filename     | num_to_words.cpp                                            |
| EntryPoint   | main                                                        |
| Purpose      | Convert a number to its words representation                |
| Inputs       | A number (double)                                           |
| Outputs      | Number in words                                             |
| Dependencies | std libs                                                    |
| By Name,Date | T.Sciple, 07/12/2026                                        |

Compile Linux:
g++ -std=c++17 -o num_to_words num_to_words.cpp
Compile Windows:
g++ -std=c++17 -o num_to_words.exe num_to_words.cpp
*/

#include <iostream>
#include <string>
#include <cmath>

std::string numToWords(double MyNumber);
std::string Place[] = {"", "Thousand", "Million", "Billion", "Trillion"};
std::string GetHundreds(std::string MyNumber);
std::string GetTens(std::string TensText);
std::string GetDigit(char Digit);
void pauseConsole();

int main() {
    double inputNumber;
    std::cout << "Enter a number: ";
    if (!(std::cin >> inputNumber)) {
        std::cout << "Invalid input." << std::endl;
        return 1;
    }
    
    std::string result = numToWords(inputNumber);
    std::cout << "Number in words: " << result << std::endl;
    
    pauseConsole();
    return 0;
}

std::string numToWords(double MyNumber) {
    if (MyNumber < 0) return "Negative numbers not supported";
    
    // 1. Separate dollars and cents using math instead of std::to_string
    long long dollarPart = static_cast<long long>(std::floor(MyNumber));
    int centsPart = static_cast<int>(std::round((MyNumber - dollarPart) * 100));

    std::string Dollars, Temp;
    int Count = 0;

    // 2. Process dollar groups (thousands, millions, etc.)
    if (dollarPart == 0) {
        Dollars = "Zero Dollars";
    } else {
        long long remainingDollars = dollarPart;
        while (remainingDollars > 0) {
            int chunk = remainingDollars % 1000;
            Temp = GetHundreds(std::to_string(chunk));
            
            if (!Temp.empty()) {
                std::string segment = Temp + (!Place[Count].empty() ? " " + Place[Count] : "");
                if (Dollars.empty()) {
                    Dollars = segment;
                } else {
                    Dollars = segment + " " + Dollars;
                }
            }
            remainingDollars /= 1000; // Integer division avoids infinite loop
            Count++;
        }
        
        if (Dollars == "One") {
            Dollars = "One Dollar";
        } else {
            Dollars += " Dollars";
        }
    }

    // 3. Process cents
    std::string CentsStr = "";
    if (centsPart > 0) {
        // Pad to 2 digits (e.g., 5 cents becomes "05")
        std::string centsText = std::to_string(centsPart);
        if (centsText.length() < 2) centsText = "0" + centsText;
        
        Temp = GetTens(centsText);
        if (centsPart == 1) {
            CentsStr = " and One Cent";
        } else {
            CentsStr = " and " + Temp + " Cents";
        }
    } else {
        CentsStr = " and No Cents";
    }

    return Dollars + CentsStr;
}

std::string GetHundreds(std::string MyNumber) {
    std::string result;
    int num = std::stoi(MyNumber);
    if (num == 0) return result;

    // Pad to exactly 3 digits
    MyNumber = std::string(3 - MyNumber.length(), '0') + MyNumber;

    if (MyNumber[0] != '0') {
        result = GetDigit(MyNumber[0]) + " Hundred";
    }

    // Process tens and ones together if they exist
    if (MyNumber[1] != '0' || MyNumber[2] != '0') {
        if (!result.empty()) result += " ";
        result += GetTens(MyNumber.substr(1));
    }
    
    return result;
}

std::string GetTens(std::string TensText) {
    std::string result;
    if (TensText[0] == '1') {
        switch (std::stoi(TensText)) {
            case 10: result = "Ten"; break;
            case 11: result = "Eleven"; break;
            case 12: result = "Twelve"; break;
            case 13: result = "Thirteen"; break;
            case 14: result = "Fourteen"; break;
            case 15: result = "Fifteen"; break;
            case 16: result = "Sixteen"; break;
            case 17: result = "Seventeen"; break;
            case 18: result = "Eighteen"; break;
            case 19: result = "Nineteen"; break;
            default: break;
        }
    } else {
        switch (TensText[0]) {
            case '2': result = "Twenty"; break;
            case '3': result = "Thirty"; break; break;
            case '4': result = "Forty"; break;
            case '5': result = "Fifty"; break;
            case '6': result = "Sixty"; break;
            case '7': result = "Seventy"; break;
            case '8': result = "Eighty"; break;
            case '9': result = "Ninety"; break;
            default: break;
        }
        if (TensText[1] != '0') {
            if (!result.empty()) {
                result += "-";
            }
            result += GetDigit(TensText[1]);
        }
    }
    return result;
}

std::string GetDigit(char Digit) {
    switch (Digit) {
        case '1': return "One";
        case '2': return "Two";
        case '3': return "Three";
        case '4': return "Four";
        case '5': return "Five";
        case '6': return "Six";
        case '7': return "Seven";
        case '8': return "Eight";
        case '9': return "Nine";
        default: return "";
    }
}

void pauseConsole() {
    #if defined(_WIN32) || defined(_WIN64)
        std::system("pause");
    #else
        std::cout << "Press Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    #endif
}