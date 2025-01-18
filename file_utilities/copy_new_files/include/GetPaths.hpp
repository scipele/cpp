//filename: GetPaths.hpp

#pragma once
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "FilePropGatherer.hpp"


class GetPaths {

public:
    std::wstring copy_from_path;
    std::wstring copy_to_path;
    int GetPathLocsAndValidate();
    void printFileDataHeaderInfo();         
    void printFileAndFolderInfo(FilePropGatherer& OrigFiles, FilePropGatherer& NewFiles);
   

private:
    void PrintPathInfo(std::wstring& msg, std::wstring& tmp_path);
    std::string FormatWithCommas(size_t num);
    int IsPathValid (const std::wstring& tmp_path);
};