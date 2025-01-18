//| Item	     | Documentation Notes                                         |
//|--------------|-------------------------------------------------------------|
//| Filename     | x.vba                                                       |
//| EntryPoint   | main                                                        |
//| Purpose      | copy over only new files                                    |
//| Inputs       | hard coded paths:                                           |
//|              |    Place Original Files Here-> c:\t\orig                    |
//|              |    Place New Files Here-> c:\t\new                          |
//| Outputs      | file log to csv                                             |
//| Dependencies | see classes in #includes here                               |
//| By Name,Date | T.Sciple, 1/11/2025                                         |

#include <iostream>
#include <locale>
#include <codecvt>
// My Classes
#include "../include/GetPaths.hpp"
#include "../include/FilePropGatherer.hpp"
#include "../../../misc/Class/Timer.hpp"


int main() {

    // 1. Start Timer
    TimerCls timer;
    timer.start();
    
    // 2. Get hard coded paths and validate that they exist
    GetPaths pth;
    pth.GetPathLocsAndValidate();

    // 3. Initialize FilePropGather class and return file counts for 
    FilePropGatherer OrigFiles(pth.copy_to_path);
    FilePropGatherer NewFiles(pth.copy_from_path);

    // 4. Get the counts of the files
    int both_counts = OrigFiles.fileCount + NewFiles.fileCount;

    // 5. Print File path data and file statistics
    pth.printFileDataHeaderInfo();
    pth.printFileAndFolderInfo(OrigFiles, NewFiles);

    // 6. Get file parent paths and file names
    OrigFiles.getFileProperties();
    NewFiles.getFileProperties();

    // 7. Output to csv
    OrigFiles.OutputToCSV(L"log_orig.csv");
    NewFiles.OutputToCSV(L"log_new.csv");

    // 8. End the Timer class
    //std::cout << std::endl;
    timer.end();

    // 9. Pause the console window before exiting
    system("pause");
    return 0;
}