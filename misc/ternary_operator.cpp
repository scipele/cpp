// ************ MAIN PROGRAM ***************************************************
//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | ternary_operator.cpp                                        |
//| EntryPoint   | main                                                        |
//| Purpose      | give two examples of using ternary operator                 |
//| Inputs       | n/a                                                         |
//| By Name,Date | T.Sciple, 01/19/2025                                        |

#include <iostream>

// Function Prototype
int confirmAdultOrMinorFromAge();
int set_min_width (int& width_input);


int main(int argc, char const *argv[])
{
    // Example 1
    confirmAdultOrMinorFromAge();
    
    // Example 2
    int width_input = 1;    // use 1 to enter the loop
    int width_used = set_min_width(width_input);
        std::cout << "Width Used: " << width_used;

    return 0;
}


int set_min_width (int& width_input)  {
    int min_width = 8;
    return (width_input > min_width) ? width_input : min_width;
    //               |                      |            |__  return if false
    //               |                      |__  return if true
    //               |__ if this is true then
}


int confirmAdultOrMinorFromAge() {
    int age = 20;
    std::string status = (age >= 18) ? "Adult" : "Minor";
    std::cout << "Status: " << status << std::endl; 
    return 0;
}
