#include <string>
#include <iostream>
#include "../include/SteelShapes.hpp"

int main(int argc, char const *argv[])
{

    std::string loopAgain;
    do {
        SteelShapes stl;

        //Keep looping or 'q' to quit
        std::cout << "\n" << "Enter to Continue, q to quit ";
        std::getline(std::cin, loopAgain);

    } while (loopAgain != "q");

    return 0;
}