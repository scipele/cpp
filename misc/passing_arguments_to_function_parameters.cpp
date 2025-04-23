#include<iostream>
#include<string>
#include <cmath>

//declare functions above so they are recognized by the compiler
void area_of_circle_byVal(double dia);
void area_of_circle_byRef(double& dia);
const double PI = 3.14159265358979323846;

/* 
in C++, the terms "variables," "parameters," and "arguments" have specific meanings within the context of the language:

Variables:   In C++, variables are named storage locations in memory that hold data. They are declared with a specific 
                data type and can be assigned values that belong to that type. Variables can be modified and accessed within the scope in which they are declared.

Parameters:     Parameters are variables declared in a function's parameter list. They act as placeholders for values that are passed
                to the function when it is called. Parameters are defined in the function's declaration or definition and specify the
                type and name of the values that the function expects to receive.

Arguments:      Arguments are the actual values that are passed to a function when it is called. They correspond to the parameters of
                the function and provide the data that the function operates on. When a function is called, the arguments are passed
                in the order specified by the function's parameter list.*/

int main() {
    //declare dia and get user input
    double dia;
    std::cout << "Scope (Main):\n";    
    std::cout << "\tInput diameter of a circle to calculate the area: ";
    std::cin >> dia;
    std::cout << "\tin main function the diameter variable is named 'dia'\n";    
    std::cout << "\tHexidecimal Memory Address of the variable 'dia' is: " << &dia << "\n";

    //call the by Val function Version
    area_of_circle_byVal(dia);
    std::cout << "Scope returned to (Main):\n";    
    std::cout << "\tNote that the Diameter passed by value was changed in the function but not in the main\n";
    std::cout << "\tDia = " << dia << " note that the value was not changed in main\n";

    
    //call the by Reference function Version
    area_of_circle_byRef(dia);
    std::cout << "Scope returned to (Main):\n";    
    std::cout << "\tNote that the Diameter passed by reference was changed in the function and also in the main\n";
    std::cout << "\tbecause when passing by Ref you are passing the location of the item in memory\n";
    std::cout << "\tDia = " << dia << " within the main function also\n";
    
    system("pause");

return 0;
}


void area_of_circle_byVal(double diameter) {

    double area = PI / 4 * pow(diameter, 2);
    std::cout << "\nScope (area_of_circle_byVal):\n";
    std::cout << "\tNote that the parameter is defined as a different name 'diameter' in the function 'area_of_circle_byVal'\n";    
    std::cout << "\twhich is the usual convention when passing an argument to a parameter by value\n";        
    std::cout << "\tdiameter = " << diameter << ", and area = " << area << "\n";
    std::cout << "\tHexidecimal Memory Address of the variable 'diameter' is: " << &diameter << "\n";
    std::cout << "\tnote that this address will be different than the variable dia in main since argument was passed by value\n";

    // now divide diamters by 2 and recalc area
    diameter = diameter / 2;

    area = PI / 4 * pow(diameter, 2);
    std::cout << "\tdiameter = " << diameter << ", and area = " << area << "\n\n";
}

void area_of_circle_byRef(double& dia) {  // The '&' operator placed behind the the type is what indica7tes the variable as a by reference pass
    double area = PI / 4 * pow(dia, 2);

    std::cout << "\nScope (area_of_circle_byRef):\n";
    std::cout << "\tNote that the ampersand '&' sign is used immeditely following the 'double' which indicates that the argument will be passed\n";
    std::cout << "\tby reference which means that the memory location is being passed\n";
    std::cout << "\tdia: " << dia << ", and area = " << area << "\n";
    std::cout << "\tHexidecimal Memory Address of the variable 'dia' is: " << &dia << "\n";
    std::cout << "\tnote that this address will be the same as the variable 'dia' in main since argument was passed by reference\n";

    // now divide diamters by 4 and recalc area
    dia = dia / 4;
    area = PI / 4 * pow(dia, 2);
    std::cout << "\tdia: " << dia << ", and area = " << area << "\n\n";
}
