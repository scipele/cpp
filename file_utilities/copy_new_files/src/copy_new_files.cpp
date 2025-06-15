//| Item	     | Documentation Notes                                         |
//|--------------|-------------------------------------------------------------|
//| Filename     | copy_new_files.cpp                                          |
//| EntryPoint   | main                                                        |
//| Purpose      | copy over only new files, and document duplicates           |
//| Inputs       | hard coded paths:                                           |
//|              |    Place Original Files Here-> c:\t\1_orig                  |
//|              |    Place New Files Here-> c:\t\3_new_files                  |
//| Outputs      | file log to csv                                             |
//| Dependencies | see classes in #includes here                               |
//| Compile Notes| make sure to compile all files in work directory            |
//|              | see tasks.json
//| By Name,Date | T.Sciple, 6/14/2025                                         |

#include <iostream>
#include <locale>
#include <codecvt>
// My Classes
#include "../include/GetPaths.hpp"
#include "../include/FilePropGatherer.hpp"
#include "../include/CompareFiles.hpp"
#include "../../../misc/Class/Timer.hpp"


int main() {

    // Start Timer
    TimerCls timer;
    timer.start();
    
    // Get hard coded paths and validate that they exist
    GetPaths pth;
    pth.GetPathLocsAndValidate();

    // Initialize FilePropGather class and return file counts for 
    FilePropGatherer OrigFiles(pth.copy_to_path);
    FilePropGatherer NewFiles(pth.copy_from_path);

    // Print File path data and file statistics
    pth.printFileDataHeaderInfo();
    pth.printFileAndFolderInfo(OrigFiles, NewFiles);

    // Get file parent paths and file names
    OrigFiles.getFileProperties();
    NewFiles.getFileProperties();

    // Create separate logs stored as csv format "|" delimeted
    OrigFiles.OutputToCSV(L"log_orig.csv");
    NewFiles.OutputToCSV(L"log_new.csv");

    // Instantiate CompareFiles Class to compare and copy new files
    CompareFiles cpyNew(OrigFiles, NewFiles);
    cpyNew.FindNewFiles();
    
    // End the Timer class
    timer.end();

    // Pause the console window before exiting
    system("pause");
    return 0;
}