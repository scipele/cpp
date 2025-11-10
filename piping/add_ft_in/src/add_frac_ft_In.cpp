// filename:        add_frac_ft_In.vba
//
// Purpose:         Add Feet, Inches, and Fractional Values in Decimal Fee
//
// Dependencies:    Class - FracConv, 
//                  Method or Class Member Function '.FracToDecFeet'
//
// By:  T.Sciple, 09/11/2024

#include <iostream>
#include <string>
#include <vector>
#include "../../FracConv/include/FracConvert.hpp"

// Function Prototypes
std::string get_user_input();

int main() {
    std::cout << "Enter Fractional Feet Inches and Fractions (ie 43'-7 7/16\") :\n"
              << "'d' for done :";
    std::string str;
    int multiplier;

    // instantiate the class FracConvert
    FracConvert conv;

    while(str != std::string("d")) {
        multiplier=1;  // set as default
        //get user entry
        str = get_user_input();

        if(str.empty()) {
            std::cout << "nothing entered";
            continue;
        }

        // Handle negative number which will essentially be subtracted
        if ( str[0] == '-') {
            multiplier = -1;
        } 

        double dec_ft = conv.FracToDecFeet(str) * multiplier;
        double dec_ft_sum;
        dec_ft_sum += dec_ft;

        std::cout << "Running Sum: " << dec_ft_sum << "\n";
    };
   
    system("pause");
}

std::string get_user_input() {
    std::cout << "entry-> ";
    std::string tmp_str;
    std::getline(std::cin, tmp_str);
    return tmp_str;
}