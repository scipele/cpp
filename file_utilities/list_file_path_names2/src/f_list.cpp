//| Item	     | Documentation Notes                                         |
//|--------------|-------------------------------------------------------------|
//| Filename     | f_list.cpp                                                  |
//| EntryPoint   | main                                                        |
//| Purpose      | list files, and parent path                                 |
//| Inputs       | user entered path in GetPath Class                          |
//| Outputs      | file log to csv                                             |
//| Dependencies | see classes in #includes here                               |
//| By Name,Date | T.Sciple, 1/30/2026                                         |
// Compile Linux -> /home/ts/dev/cpp/file_utilities/list_file_path_names2/src/g++ -std=c++17 -o f_list *.cpp class/*.cpp
//      

#include <iostream>
#include <locale>
#include <codecvt>
// My Classes
#include "../include/GetPath.hpp"
#include "../include/FilePropGatherer.hpp"
#include "../../../misc/Class/Timer.hpp"

// Function prototype
void pause_console();


int main() {

    // Start Timer
    TimerCls timer;
    
    // Get hard coded paths and validate that they exist
    GetPath pth;
    if (pth.GetPathLocsAndValidate() != 0) {
        std::cerr << "Program terminated due to invalid path(s)." << std::endl;
        system("pause");
        return -1; // Exit the program with an error code
    }

    timer.start();
    
    // Initialize FilePropGather class and return file counts for 
    FilePropGatherer ListFiles(pth.list_files_path);

    // Print File path data and file statistics
    pth.printFileDataHeaderInfo();
    pth.printFileAndFolderInfo(ListFiles);

    // Get file parent paths and file names
    ListFiles.getFileProperties();

    // Create separate logs stored as csv format "|" delimeted
    ListFiles.OutputToCSV(L"log_orig.csv");
   
    // End the Timer class
    timer.end();

    // Pause the console window before exiting
    pause_console();
    return 0;
}


void pause_console() {
    #ifdef _WIN32
        system("pause");
    #else
        std::cout << "Press Enter to continue...";
        std::cin.get();
    #endif
}