#include <iostream>
#include <string>
#include <cctype>  //Provides functions for character handling, such as std::tolower.

// Function to convert a string to lowercase
std::string toLowerCase(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(c);  // replaces each character in the ByRef Operator '&'str
    }
    return result;
}

std::string toUpperCase(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::toupper(c);
    }
    return result;
}

std::string toProperCase(const std::string& str) {
    std::string result = str;
    bool capitalizeNext = true;
    for (char& c : result) {
        if (std::isalpha(c)) {
            if (capitalizeNext) {
                c = std::toupper(c);
                capitalizeNext = false;
            } else {
                c = std::tolower(c);
            }
        } else {
            capitalizeNext = true;
        }
    }
    return result;
}

// Function to pause the console window until a key is pressed
void pauseConsole() {
    std::cout << "Press Enter key to continue...";
    std::string input;
    std::getline(std::cin, input); // Wait for the user to press Enter
}
 
int main() {
    std::string inp;
    std::cout << "Enter a String to convert to different Cases (Lower, Upper, Proper): ";
    std::getline(std::cin, inp);  

    std::string strlower  = toLowerCase(inp);
    std::string strUpper = toUpperCase(inp);
    std::string strProper = toProperCase(inp);
    
    std::cout << "all lower Case:  " << strlower << "\n";
    std::cout << "all upper Case:  " << strUpper << "\n";
    std::cout << "all proper Case: " << strProper << "\n";
    
    pauseConsole();

    return 0;
}