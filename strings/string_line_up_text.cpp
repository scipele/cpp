#include <iostream>
#include <string>
#include <iomanip> // Include the header for setw()

int main() {
    
    std::string a;
    a = "12";
    std::string b;
    b = "34652";    

//  This example allows a width of 20 characters displaying the "a or the 'b" variables,
//  then the other text is lined up after that set width
    std::cout << std::setw(20) << std::left << a << "Line up text here"<< "\n";
    std::cout << std::setw(20) << std::left << b << "Line up text here"<< "\n";    

return 0;
}

