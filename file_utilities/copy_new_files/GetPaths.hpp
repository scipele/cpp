#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstdint>
#include <windows.h>
#include <tchar.h>
#include <string>
#include <sstream>
#include <strsafe.h>
#include <codecvt>
#include <vector>

class GetPaths {

public:
    struct paths {
        std::wstring copy_from_path;
        std::wstring copy_to_path;
        std::wstring log_file_path;
    };

    paths pth;

    int UserInput () {
        // get user entered path
        std::cout << "Enter the path to copy files from\n"
                << "   Example Entry 'c:\\myFolder'\n"
                << "   Or press 'c' to cancel\n"
                << "   User entry here ============> ";

        std::getline(std::wcin, pth.copy_from_path );

        // Allow user to cancel execution by entering 'c'
        if (pth.copy_from_path == L"c") {
            std::cout << "Execution cancelled by user." << std::endl;
            system("pause");
            return 0; // Exit gracefully
        }

        // Check if the entered path exists
        if (!std::filesystem::exists(pth.copy_from_path)) {
            std::cerr << "Error: The specified path does not exist." << std::endl;
            system("pause");
            return -1; // Exit with error
        }
        int result = SetupOutputFile();
        return 0;
    }

private:

    int SetupOutputFile () {
        // Define the output folder and filename
        const std::string outputFolderPath = "c:\\temp";
        const std::string outputFileName = "fileList.csv";
        const std::string outputFullFile = outputFolderPath + "\\" + outputFileName;

        // Create the output folder if it doesn't exist
        if (!std::filesystem::exists(outputFolderPath)) {
            if (!std::filesystem::create_directory(outputFolderPath)) {
                std::cerr << "Error: Unable to create output folder." << std::endl;
                return 1;
            }
        }

        // hard code where to place the filename
        std::string outputFile = "c:\\temp\\log_file.txt";
        return 0;
    }   
};


