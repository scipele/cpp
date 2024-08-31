// Filename:    main.cpp
//
// Purpose:     This is an example usage of the PipeData Class
//              - See header file 'PipeData.hpp' for basic outline of the class
//              - See the implementation file 'PipeData.cpp' for Public and Private Member Function Details
//
// Dependencies:    Header/Implementation Files             Class Name
//                  PipeData.hpp                            PipeData
//                  PipeData.cpp                            PipeData
//
//              The following directive must be used in the '"args": [' section of the 'tasks.json' file so that the
//              compiler will compile all the files in the current path
//                  - Placement:  put the following after the "-g", argument
//                      "*.cpp",  // This compiles all .cpp files in the current directory
//                      comment this out -> // "${file}",  which tells the compiler to compile only the current file
//
//  By:         T.Sciple, 8/31/2024


#include <iostream>
#include "PipeData.hpp" // Header file definition for PipeData Class, note that the implementation file is PipeData.cpp

int main(int argc, char const *argv[])
{
    std::string loopAgain;
    do
    {
        // Instantiate an Object PipeData
        PipeData pd;

        // Call Member Function / Method  which will get the asize and schedule from the user
        pd.GetUserInput();

        // Call the following Member Function / Method which will print the detailed data
        pd.PrintPipeData();

        //Keep looping or 'q' to quit
        std::cout << "Enter to Continue, q to quit ";
        std::getline(std::cin, loopAgain);

    } while (loopAgain != "q");

    //pause for user input to prevent window from closing
    system("pause");

    return 0;
}
