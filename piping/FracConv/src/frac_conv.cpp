#include <iostream>
#include <string>
#include <iomanip>
#include "../include/FracConvert.hpp"

int main() {
    // instantiate the class FracConvert
    FracConvert conv;
    
    // optional command line input line
    for ( int i=0; i<25; i++) std::cout << " \n";

    std::string input;
    std::cout << "Convert Fractional Feet, Inches and Fractions to Decimal (ie 43'-7 7/16\") : ";
    std::getline(std::cin, input);
    
    std::cout << std::endl;
    std::cout << std::left  << "+----------------+----------------+----------------+----------------+" << "\n"
                            << "| " << std::setw(15) << "Input"
                            << "| " << std::setw(15) << "Dec Feet"
                            << "| " << std::setw(15) << "Dec Inch"
                            << "| " << std::setw(15) << "Millimeters" << "|" << "\n"
                            << "+----------------+----------------+----------------+----------------+" << "\n";
    
    // call Class methods and return converted units
    std::cout << std::setprecision(6) << std::left << "| " << std::setw(15) << input
                            << "| " << std::setw(15) << conv.FracToDecFeet(input) 
                            << "| " << std::setw(15) << conv.FracToDecInch(input) 
                            << "| " << std::setw(15) << conv.FracToDecMillimeter(input) << "|" << "\n"
                            << "+----------------+----------------+----------------+----------------+" << "\n";
    system("pause");
}