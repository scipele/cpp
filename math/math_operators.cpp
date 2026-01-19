#include <iostream>
#include <cmath>
#include <stdlib.h>
const double PI = 3.14159265358979323846;

//Function Prototypes
void PythagExample();
void ConvertFtInDecFtExample();
void TrigFunctionExample();

int main() {
    PythagExample();
    ConvertFtInDecFtExample();
    TrigFunctionExample();

    // Pause the console window before exiting
    std::cin.get();
    return 0;
}

void PythagExample() {  // This is an example of pythagorean theorem [ c = sqrt(a^2+b^2) ] //
    double a = 1;
    double b = sqrt(4);  // square root of three
    double c = sqrt(pow(a, 2) + pow(b, 2));  // sqrt of ( a^2 + b^2 )
    std::cout << "\n\n1. Example of Solving Triange using pythagorean theorem:" << "\n";
    std::cout << "\tGiven a = " << a << "\n";
    std::cout << "\tGiven b = " << b << "\n";
    std::cout << "\tCalc  c = " << c;
}

void ConvertFtInDecFtExample() {  // next example is conversion of feet and inches into decimal feet
    double feet = 3;
    double inches = 7;
    double frac_numerator = 5;
    double frac_denominator = 16;
    double dec_feet = feet + (inches + (frac_numerator / frac_denominator))/12;
    std::cout << "\n\n2. Example of Converting Feet and Inches to Decimal:" << "\n";
    std::cout << "\tConvert " << feet << "'-" << inches << " " << frac_numerator << "/" << frac_denominator << "\" to" << "\n";    
    std::cout << "\tdecimal feet = " << dec_feet;
}

void TrigFunctionExample() {   // next example is using a trig function
    double opp_len = 1;
    double ang_deg = 45.0;
    double ang_rad = ang_deg * PI / 180.0; // convert degrees to radians
    double sine_ang = std::sin(ang_rad);  // sin, cos, tan, asin, acos, atan Functions in the math
    double hyp_len = opp_len / sine_ang;
    std::cout << "\n\n3. Trigonomotry Example:" << "\n";
    std::cout << "\tGiven opp_len = " << opp_len << "\n";
    std::cout << "\tGiven ang_deg = " << ang_deg << "\n";
    std::cout << "\tHypotenuse = " << hyp_len << "\n\n";
}
