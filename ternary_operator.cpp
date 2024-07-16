#include <iostream>

// Function Prototype
int set_min_width (int& width_input);


int main(int argc, char const *argv[])
{
    
    
    int width_input = 1;    // use 1 to enter the loop
    int width_used;

    std::cout << "Enter Zero to Exit Loop ";

    while (width_input != 0) {
        std::cout << "\n\nEnter Print Width: ";
        std::cin >> width_input;

        width_used = set_min_width(width_input);
        std::cout << "Width Used: " << width_used;

    }

    return 0;
}


int set_min_width (int& width_input)  {

    int min_width = 8;
    return (width_input > min_width) ? width_input : min_width;
    //               |                      |            |
    //               |                      |            |__  return if false
    //               |                      |
    //               |                      |__  return if true
    //               |
    //               |__ if this is true then
    
}
