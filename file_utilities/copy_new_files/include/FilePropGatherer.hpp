#pragma once
#include <filesystem>
#include <windows.h>
#include <bcrypt.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#pragma comment(lib, "bcrypt.lib")
#include "../../../misc/Class/ProgressBar.hpp"
   

class FilePropGatherer {
public:
    struct FileInfo {
        std::string hash_code;
        std::wstring file_parent_path;
        std::wstring file_name;
    };
    
    std::vector<FileInfo> vecFileInfo;
    
    // Constructor that initializes the user_path member variable
    FilePropGatherer( const std::wstring& input_user_path)
                        : user_path(input_user_path),
                          fileCount(0),
                          folderCount(0),
                          prg(0)  // Temporary initialization since fileCount isn't known yet
                    {
                        getFileCount();
                        getFolderCount();
                        prg = ProgressBar(fileCount);  // Reinitialize prg with correct fileCount
                    }

    // Public member function prototypes
    int getFileProperties();
    int getFileCount();
    int getFolderCount();
    void OutputToCSV();

    // Public member variables
    int fileCount;
    int folderCount;

private:
    std::wstring user_path;
    std::string get_current_date();
    ProgressBar prg;  // Declare ProgressBar class
    std::string GetHashCode(const std::string& filePath);
    std::string hashFileWithSHA1(const std::string& filePath);
};