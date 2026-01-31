//| Item	     | Class Documentation Notes                                   |
//|--------------|-------------------------------------------------------------|
//| Filename/type| GetPath.hpp / Class header file                            |
//| EntryPoint   | instantiated from main                                      |
//| By Name,Date | T.Sciple, 8/30/2025                                         |

#pragma once
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "FilePropGatherer.hpp"


class GetPathInfo {

public:
    // Default constructor
    GetPathInfo();

    // Destructor
    virtual ~GetPathInfo();
    
    std::wstring list_files_path;
    int GetPathLocsAndValidate();
    void printFileDataHeaderInfo();         
    void printFileAndFolderInfo(FilePropGatherer& OrigFiles);





private:
    void PrintPathInfo(std::wstring& msg, std::wstring& tmp_path);
    std::string FormatWithCommas(size_t num);
    int IsPathValid (const std::wstring& tmp_path);
};