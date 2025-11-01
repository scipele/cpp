//| Item	     | Class Documentation Notes                                   |
//|--------------|-------------------------------------------------------------|
//| Filename/type| CompareFiles.hpp / Class header file                        |
//| EntryPoint   | instantiated from main                                      |
//| By Name,Date | T.Sciple, 1/18/2025                                         |

#pragma once
#include <iostream>
#include <chrono>
#include <codecvt>
#include <locale>
#include <optional>
#include <string>
#include <vector>
#include "FilePropGatherer.hpp"


class CompareFiles {

public:
    // Constructor taking references to FilePropGatherer instances
    CompareFiles(FilePropGatherer& originalFiles, FilePropGatherer& newFiles)
        : origFiles(originalFiles), newFiles(newFiles) {
    }

    void FindNewFiles();

private:
    enum fileCateg {
        in_new_no_duplicate = 0,
        in_new_is_duplicate = 1,
        in_orig_no_duplicate = 2
    };
    // References to FilePropGatherer instances
    FilePropGatherer& origFiles;
    FilePropGatherer& newFiles;

    // member function prototypes
    std::string getStrFromWstr(std::wstring& wstr);
    std::string getStrFromConstWstr(const std::wstring& wstr);
    void writeHeaderLineToCsv(std::ofstream& file);
    void writeDataLineToCsv(int j, int id, std::ofstream& file, std::string& hash, fileCateg type);
    std::optional<std::ofstream> getFileHandle();
    std::wstring getCopySourceFullPath(int i);
    std::wstring getCopyDestinationFullPath(int i);
    std::wstring getCopyDestinationSeparateFullPath(int i);
    int PrepForFileCopy();
    void copyOverNewFile(int i);
    std::wstring get_current_date();
    std::wstring stripExtension(const std::wstring& filename);

    // member variables
    std::wstring orig_path;
    std::wstring new_path;
    int unique_files_count;    
    int copied_count;


};