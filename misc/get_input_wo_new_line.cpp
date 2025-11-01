#include <iostream>
#include <string>
#include <conio.h> // _getch() on Windows

// Function Prototypes
int GetIntInputWoNewLine();
double GetDoubleInputWoNewLine();

int main() {

    std::cout << "\n\n";
    int num_i = GetIntInputWoNewLine();
    std::cout << ", Note this is on the same line, You entered: " << num_i << "\n\n";
    
    double num_d = GetDoubleInputWoNewLine();
    std::cout << ", Note this is on the same line, You entered: " << num_d << "\n\n";

    return 0;
}

int GetIntInputWoNewLine() {
    std::string str;
    std::cout << "Enter an integer: ";
    char c = _getch();  // _getch() captures keyboard input without echo, reset in loop
    while (c != '\r') {
        str += c;
        std::cout << c;  // This shows you input on the screen
        c = _getch();   
    } 
    return std::stoi(str);  // Convert the input string to an integer
}

double GetDoubleInputWoNewLine() {
    std::string str;
    std::cout << "Enter an decimal number to be converted to double: ";
    char c = _getch();  // _getch() captures keyboard input without echo, reset in loop
    while (c != '\r') {
        str += c;
        std::cout << c;  // This shows you input on the screen
        c = _getch();   
    } 
    return std::stod(str);  // Convert the input string to an double
}