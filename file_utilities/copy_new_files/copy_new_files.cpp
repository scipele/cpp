// filename:    copy_new_files
//
// Purpose:     copy over only files if they are new files determined as follows
//
// How if works:
//              1. Get Path information from the user
//                  - copy from path                     
//                  - copy to path                     
//
//              2. Return a vector of File Properties and Hash
//                 Copy over new files that are confirmed to have a new hash code
//              SHA-1 Hash creates a 40 Hexidecimal Digit unique id for the binary content of any file
//              Skips file larger than 50 Mb
//
// Dependencies:
//            +-------------------+--------------+-----------------+
//            |  Class File Name  |  Class Name  |  Public Method  |
//            +-------------------+--------------+-----------------+
//            |  GetPaths.hpp     |  GetPaths    |  Public Method  |
//
//
//
// Output:    The log file listing is placed in the c:\t\log_file.txt
// By:          Tony Sciple  08/15/2024

#include "GetPaths.hpp"
#include "SafeDirIter.hpp"

// Function Declarations
int listFilesRecursively(const std::filesystem::path& path, const std::string& outputFile, int& totalFiles);
void updateProgress(int current, int total);

int main() {

    // call a class to get path names from the user and validate that the paths are legit
    GetPaths pth;
    pth.GetPathLocs();

    //std::cout << "Check Path from User" << 


    int totalFiles = 0;
//    listFilesRecursively(path, outputFile, totalFiles);

    // Display number of files processed
//    std::cout << "\n\nFile information has been written to " << outputFile << "\n"
//              << "for " << totalFiles << " files" << "\n";
    
    
    // Pause the console window before exiting
    system("pause");

    return 0;
}

int getSafeFileCount(const std::filesystem::path& path) {
    // Get total file count for progress indication
    int fileCount = 0;
    std::error_code ec;
    for (auto dir = SafeDirIter::begin(path, ec); dir != SafeDirIter::end_iter(); ++dir) {
        if ((*dir).is_regular_file(ec)) {
            fileCount++;
        }
    }    
    return fileCount;
}

int listFilesRecursively(const std::filesystem::path& path, const std::string& outputFile, int& totalFiles) {
    std::wofstream outFile(outputFile);
    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open output file." << std::endl;
        return 1;
    }
    // place the header field names at the beginning of the file
    outFile << "row|parent_path|file_name|file_Size|sha1_hash|date_time|count" << "\n";
    
    // Get total file count for progress indication
    totalFiles = getSafeFileCount(path);

    // Iterate over each entry in the directory recursively
    int currentFile = 0;
    int rowCount=0;
    std::error_code ec;
    for (auto dir = SafeDirIter::begin(path, ec); dir != SafeDirIter::end_iter(); ++dir) {
        if ((*dir).is_regular_file(ec)) {
            currentFile++;
            updateProgress(currentFile, totalFiles);
            std::cout.flush();

            outFile << currentFile << L"|" 
                    << dir.get_parent_path() << L"|" 
                    << dir.get_filename() << L"|"
                    << dir.get_file_size() << L"|"
                    << dir.sha1FileHash() << L"|" 
                    << dir.getLastWriteTime() << L"|\n";
        }
        // troubleshooting only -> std::wcout << currentFile << L"." << dir.get_filename() << "\n";
    }
    return 0;
}

void updateProgress(int current, int total) {
    const int barWidth = 100;
    float progress = (float)current / total;
    int pos = barWidth * progress;

    std::cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << "%\r";
    std::cout.flush();
}
