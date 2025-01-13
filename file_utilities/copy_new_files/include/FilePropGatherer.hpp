#pragma once
#include <filesystem>
#include <iostream>
#include <string>

class FilePropGatherer {
public:
    struct FileInfo {
        std::string hash_code;
        std::wstring file_parent_path;
        std::wstring file_name;
    };

    // Constructor that initializes the user_path member variable
    FilePropGatherer(const std::wstring& input_user_path) : user_path(input_user_path) {
        // Go ahead and count the files and the folders
        getFileCount();
        getFolderCount();
    }

    // Public member function prototypes
    int getFileProperties();
    int getFileCount();
    int getFolderCount();

    // Pulblic member variables
    int fileCount;
    int folderCount;

private:
    std::wstring user_path;

    // Private member function prototypes
    std::string get_current_date();
};