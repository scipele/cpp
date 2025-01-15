//| Item	     | Documentation Notes                                         |
//|--------------|-------------------------------------------------------------|
//| Filename     | x.vba                                                       |
//| EntryPoint   | main                                                        |
//| Purpose      | copy over only new files                                    |
//| Inputs       | hard coded paths:                                           |
//|              |    Place Original Files Here-> c:\t\orig                    |
//|              |    Place New Files Here-> c:\t\new                          |
//| Outputs      | file log to csv                                             |
//| Dependencies | see classes in #include here                                |
//| By Name,Date | T.Sciple, 1/11/2025                                         |

#include <iostream>
#include <locale>
#include <codecvt>

#include "../include/GetPaths.hpp"
#include "../include/FilePropGatherer.hpp"
#include "../../../misc/Class/Timer.hpp"

// Function Protypes:
void printFileAndFolderInfo(GetPaths& pth, FilePropGatherer& OrigFiles, FilePropGatherer& NewFiles);
std::string FormatWithCommas(size_t num);


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
    printFileAndFolderInfo(pth, OrigFiles, NewFiles);

    // 5. Get file parent paths and file names
    OrigFiles.getFileProperties();
    NewFiles.getFileProperties();

    // Output to csv
    OrigFiles.OutputToCSV();

    // 7. End the Timer class
    std::cout << std::endl;
    timer.end();

    // 8. Pause the console window before exiting
    system("pause");
    return 0;
}


void printFileAndFolderInfo(GetPaths& pth, FilePropGatherer& OrigFiles, FilePropGatherer& NewFiles) {

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string pathA = converter.to_bytes(pth.copy_to_path);
    std::string pathB = converter.to_bytes(pth.copy_from_path);

    std::cout   << "+-------------------------+---------------+-----------------+\n"
                << "| Path Location           |  File Counts  |  Folder Counts  |\n"
                << "+-------------------------+---------------+-----------------+\n"
                << "| " 
                    << std::left << std::setw(24) << pathA << "|"
                    << std::right << std::setw(14) << FormatWithCommas(OrigFiles.fileCount) << " |"
                    << std::setw(16) << FormatWithCommas(OrigFiles.folderCount) << " |\n"
                << "| " 
                    << std::left << std::setw(24) << pathB << "|"
                    << std::right << std::setw(14) << FormatWithCommas(NewFiles.fileCount) << " |"
                    << std::setw(16) << FormatWithCommas(NewFiles.folderCount) << " |\n"
                << "+-------------------------+---------------+-----------------+" << std::endl;
}

std::string FormatWithCommas(size_t num) {
    std::string str = std::to_string(num);
    int bkw_cnt = str.length(); // initialize a backward counter to count down from string len to 0
    std::string formated_str;

    for (char c : str) {     // loops thru each character in the string
        formated_str += c;
        bkw_cnt--;  // deincrement the count by 1 after the first character which will always be there before checking for needed commas
        if (!bkw_cnt == 0) {
            if(bkw_cnt % 3 == 0) {  // checks if the mod of bkw_cnt / 3 == 0, or remainder is 0
                    formated_str += ',';
            }
        }
    }
    return formated_str;
}

