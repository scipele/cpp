// filename:    getShapeProps.cpp
// 
// Purpose:     Uses the SteelShapes Class to get properties of
//              steel shapes in a loop
// Dependencies:
//              - See SteelShapes.hpp for class header file
//              - See SteelShapes.cpp for class implementation file
//
// By:  T. Sciple, 2024-10-14

#include <string>
#include <iostream>
#include "../include/SteelShapes.hpp"

int main(int argc, char const *argv[])
{
    std::string loopAgain;
    do {
        // instantiate an instance of the SteeShapes Class which will read the proper
        SteelShapes stl;

        // Call public member function to get user shape
        stl.get_str_from_user();

        // Call public member function to print the properties
        //stl.PrintShapeProperties();

        //Keep looping or 'q' to quit
        std::cout << "\n" << "Enter to Continue, q to quit ";
        std::getline(std::cin, loopAgain);

    } while (loopAgain != "q");

    return 0;
}