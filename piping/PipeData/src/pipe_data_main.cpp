// Filename:    main.cpp
//
// Purpose:     This is an example usage of the PipeData Class
//              - See header file 'PipeData.hpp' for basic outline of the class
//              - See the implementation file 'PipeData.cpp' for Public and Private Member Function Details
//
// Dependencies:    Header/Implementation Files             Class Name
//                  PipeData.hpp (Header File)              PipeData
//                                                          PipeDataExtended
//                  PipeData.cpp (Class Implementation File)
//                                                          PipeData
//                                                          PipeDataExtended
//
// How to Compile:  
// Windows Compile:     g++ "-fdiagnostics-color=always""-g" *.cpp" "class/*.cpp" -o" pipe_data_main 
// Linux Compile:       g++ "-fdiagnostics-color=always""-g" *.cpp" "class/*.cpp" -o" pipe_data_main 
//  By:         T.Sciple, 8/31/2024


#include <iostream>
#include "../include/PipeData.hpp" // Header file definition for PipeData Class, note that the implementation file is PipeData.cpp


int main()
{
    std::string loopAgain;
    do
    {
        // Instantiate the extended property derived class so that we can get all the pipe properties
        PipeDataExtended pdExt;
        //  Prints the data from the base class 'PipeData' which automatically gets instantiated when the derived class is instantiated
        pdExt.PrintPipeData();      
        pdExt.PrintPipeDataExt();

        //Keep looping or 'q' to quit
        std::cout << "Enter to Continue, q to quit ";
        if (!std::getline(std::cin, loopAgain)) {
            std::cerr << "\nInput stream closed. Exiting.\n";
            break;
        }

    } while (loopAgain != "q");

    return 0;
}