#include <iostream>
#include <cmath>
#include <stdlib.h>


void pythag_example() {  // This is an example of pythagorean theorem [ c = sqrt(a^2+b^2) ] //
    double a = 1;
    double b = sqrt(3);  // square root of three
    double c = sqrt(pow(a, 2) + pow(b, 2));  // sqrt of ( a^2 + b^2 )

    for (int i=0; i<50; i++) std::cout << "\n";
    std::cout << "Example of Solving Triange using pythagorean theorem:" << "\n";
    std::cout << "Given a = " << a << "\n";
    std::cout << "Given b = " << b << "\n";
    std::cout << "Calc  c = " << c << "\n\n";
}

void convert_ftIn_decFt_example() {  // next example is conversion of feet and inches into decimal feet
    double feet = 3;
    double inches = 7;
    double frac_numerator = 5;
    double frac_denominator = 16;
    double dec_feet = feet + (inches + (frac_numerator / frac_denominator))/12;
    std::cout << "Convert " << feet << "'-" << inches << " " << frac_numerator << "/" << frac_denominator << "\" to" << "\n";    
    std::cout << "decimal feet = " << dec_feet << "\n\n";
}

void trig_function_example() {   // next example is using a trig function
    const double PI  = 3.14159265358979323846;
    double opp_len = 1;
    double ang_deg = 45.0;
    double ang_rad = ang_deg * PI / 180.0; // convert degrees to radians

    double sine_ang = std::sin(ang_rad);  // sin, cos, tan, asin, acos, atan Functions in the math
    double hyp_len = opp_len / sine_ang;
    std::cout << "Given opp_len = " << opp_len << "\n";
    std::cout << "Given ang_deg = " << ang_deg << "\n";
    std::cout << "Hypotenuse = " << hyp_len << "\n\n";
}

int main() {
    pythag_example();
    convert_ftIn_decFt_example();
    trig_function_example();

    return 0;
}