//| Item	     | Class Documentation Notes                                   |
//|--------------|-------------------------------------------------------------|
//| Filename/type| FilePropGatherer.hpp / Class header file                    |
//| EntryPoint   | instantiated from main                                      |
//| By Name,Date | T.Sciple, 1/11/2025                                         |

#pragma once
#include <filesystem>
#include <windows.h>
#include <bcrypt.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <string>
#pragma comment(lib, "bcrypt.lib")
#include "../../../misc/Class/ProgressBar.hpp"


class FilePropGatherer {
public:
    struct FileInfo {
        const std::string hash_code;
        const std::wstring file_parent_path;
        const std::wstring file_name;
        int hash_count;
    };
    
    std::wstring folder_parent_path;
    std::vector<FileInfo> vecFileInfo;
    std::unordered_map<std::string, int> hashes;
    
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
    int getFolderParentPath();
    int getFileCount();
    int getFolderCount();
    void OutputToCSV(const std::wstring& file_name);
    
    // Public member variables
    int fileCount;
    int folderCount;
    const std::wstring user_path;

private:
    std::string get_current_date();
    ProgressBar prg;  // Declare ProgressBar class
    std::string GetHashCode(const std::string& filePath);
    std::string hashFileWithSHA1(const std::string& filePath);
    int populate_map_with_hashes();

    int number_of_duplicate_files;
};