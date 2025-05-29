//| Item	     | Documentation Notes                                         |
//|--------------|-------------------------------------------------------------|
//| Filename     | list_files.cpp                                              |
//| EntryPoint   | main                                                        |
//| Purpose      | list file properties                                        |
//| Inputs       | hard coded path:                                           |
//|              |    Place Files Here-> c:\t\list                             |
//| Outputs      | file log to csv                                             |
//| Dependencies | see classes in #includes here                               |
//| By Name,Date | T.Sciple, 1/11/2025                                         |

#include <iostream>
#include <locale>
#include <codecvt>
// My Classes
#include "../include/GetPath.hpp"
#include "../include/FilePropGatherer.hpp"
#include "../../../misc/Class/Timer.hpp"


int main() {

    // Start Timer
    TimerCls timer;
    timer.start();
    
    // Get hard coded path and validate that they exist
    GetPath pth;
    pth.GetPathLocsAndValidate();

    // Initialize FilePropGather class and return file counts for 
    FilePropGatherer OrigFiles(pth.list_path);
    
    // Print File path data and file statistics
    pth.printFileDataHeaderInfo();
    pth.printFileAndFolderInfo(OrigFiles);

    // Get file parent path and file names
    OrigFiles.getFileProperties();
    
    // Create separate logs stored as csv format "|" delimeted
    OrigFiles.OutputToCSV(L"log_orig.csv");
   
    // End the Timer class
    timer.end();

    // Pause the console window before exiting
    system("pause");
    return 0;
}