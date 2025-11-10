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
//              The following directive must be used in the '"args": [' section of the 'tasks.json' 
//            "args": [
//                "-fdiagnostics-color=always",
//                "-g",
//                "*.cpp",  // Compile all .cpp files in the current directory
//                "class/*.cpp",    // compiles the implementation files
//                "-o",
//                "${fileDirname}\\${fileBasenameNoExtension}.exe"
//            ],

//              compiler will compile all the files in the current path
//                  - Placement:  put the following after the "-g", argument
//                      "*.cpp",  // This compiles all .cpp files in the current directory
//                      comment this out -> // "${file}",  which tells the compiler to compile only the current file
//
//  By:         T.Sciple, 8/31/2024


#include <iostream>
#include "../include/PipeData.hpp" // Header file definition for PipeData Class, note that the implementation file is PipeData.cpp


int main(int argc, char const *argv[])
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
        std::getline(std::cin, loopAgain);

    } while (loopAgain != "q");

    // Alternate try just getting basic properties with base class 'PipeData'
    // PipeData pd;
    // pd.PrintPipeData();

    return 0;
}