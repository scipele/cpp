#include <iostream>
#include <string>

//fix this later - not working correctly - sciple

std::string numToWords(double MyNumber);

std::string Place[] = {"", "Thousand", "Million", "Billion", "Trillion"};

std::string GetHundreds(std::string MyNumber);
std::string GetTens(std::string TensText);
std::string GetDigit(char Digit);
void pauseConsole();


int main() {
    double inputNumber;
    std::cout << "Enter a number: ";
    std::cin >> inputNumber;

    std::string result = numToWords(inputNumber);
    std::cout << "Number in words: " << result << std::endl;

    // Call the function to pause the console window
    pauseConsole();


    return 0;
}

std::string numToWords(double MyNumber) {
    double Cents;
    int DecimalPlace, Count = 0;
    std::string Dollars, Temp;

    DecimalPlace = std::to_string(MyNumber).find('.');
    if (DecimalPlace > 0) {
        Cents = std::stod(std::to_string(MyNumber).substr(DecimalPlace + 1));
        MyNumber = std::stoi(std::to_string(MyNumber).substr(0, DecimalPlace));
    }


    while (MyNumber > 0) {
        Temp = GetHundreds(std::to_string(static_cast<int>(static_cast<long long>(MyNumber) % 1000)));
        if (!Temp.empty()) {
            if (!Dollars.empty()) { // Check if Dollars is not empty
                Dollars = " " + Dollars; // Add a space before concatenating the term
            }
            Dollars = Temp + (!Temp.empty() && !Place[Count].empty() ? " " : "") + Place[Count] + Dollars; // Add a space between Temp and Place[Count] if both are not empty
        }
        MyNumber /= 1000;
        Count++;
    }


    if (Dollars.empty())
        Dollars = "No Dollars";
    else if (Dollars == "One")
        Dollars = "One Dollar";
    else
        Dollars += " Dollars";

    std::string CentsStr = "";
    if (Cents > 0) {
        Temp = GetTens(std::to_string(static_cast<int>(Cents)));
        if (!Temp.empty())
            CentsStr = " and " + Temp + " Cents";
        else
            CentsStr = " and One Cent";
    } else {
        CentsStr = " and No Cents";
    }

    return Dollars + CentsStr;
}

std::string GetHundreds(std::string MyNumber) {
    std::string result;
    if (std::stoi(MyNumber) == 0)
        return result;
    MyNumber = std::string(3 - MyNumber.length(), '0') + MyNumber;
    
    if (MyNumber[0] != '0') {
        result = GetDigit(MyNumber[0]) + " Hundred";
    }
    if (MyNumber[1] != '0') {
        if (!result.empty()) {  // Add space if result is not empty
            result += " ";  
        }
        result += GetTens(MyNumber.substr(1));
    } else if (MyNumber[2] != '0' && MyNumber[1] != '0') {
        if (!result.empty()) {  // Add space if result is not empty
            result += " ";  
        }
        result += GetDigit(MyNumber[2]);
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
            case '3': result = "Thirty"; break;
            case '4': result = "Forty"; break;
            case '5': result = "Fifty"; break;
            case '6': result = "Sixty"; break;
            case '7': result = "Seventy"; break;
            case '8': result = "Eighty"; break;
            case '9': result = "Ninety"; break;
            default: break;
        }
        if (TensText[1] != '0') {
            if (!result.empty()) {  // Add hyphen if result is not empty
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


// Function to pause the console window until a key is pressed
void pauseConsole() {
//    std::cout << "Press any key to continue...";
//    std::string input;
//    std::getline(std::cin, input); // Wait for the user to press Enter
    
    // Pause the console window before exiting
    system("pause");

}
