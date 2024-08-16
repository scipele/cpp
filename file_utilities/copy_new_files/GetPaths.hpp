#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <ostream>
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
    std::wstring copy_from_path;
    std::wstring copy_to_path;
    std::wstring log_file_path;

    int GetPathLocs() {
        std::cout << std::endl;
        std::vector<std::wstring> msgs = {   L"Copy From Path",
                                            L"Copy To Path",
                                            L"Log File Path" };
        std::vector<int> results(2);                                    
        results[0] = UserInput(msgs[0], this->copy_from_path);
        results[1] = UserInput(msgs[1], this->copy_to_path);

        bool chk_results = true;    // assume results are good unless reset to false
        for (auto& result : results) {
            if (result != 0) chk_results = false;   // means that an error was encountered if not 0
        }
       
        if (chk_results) {
            print_path_info(msgs[0], this->copy_from_path);
            print_path_info(msgs[1], this->copy_to_path);
        } else {
            return -1;
        }
        // return standard 0 if no errors were encountered
        return 0;
    }


private:

    void print_path_info(std::wstring& msg, std::wstring& tmp_path) {
        std::wcout  << L"Path Entered for '" 
                    << msg
                    << L"' was '"
                    << tmp_path
                    << L"' and was validated\n";
    }

    int UserInput (std::wstring msg_to_user, std::wstring& tmp_path) {
        // get user entered path
        std::wcout   << L"Enter the path to "
                    << msg_to_user << "\n"
                    << L"   Example Entry 'c:\\myFolder'\n"
                    << L"   Or press 'c' to cancel\n"
                    << L"   User entry here ============> ";

        std::getline(std::wcin, tmp_path );

        // Allow user to cancel execution by entering 'c'
        if (tmp_path == L"c") {
            std::cout << "Execution cancelled by user." << std::endl;
            system("pause");
            tmp_path = L"";  // reset the path equal to nothing
            return 0; // Exit gracefully
        }

        // Check if the entered path exists
        if (!std::filesystem::exists(tmp_path)) {
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


