//filename: GetPaths.hpp

#pragma once
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

class GetPaths {

public:
    std::wstring copy_from_path;
    std::wstring copy_to_path;

    int get_path_locs() {
        std::vector<std::wstring> msgs = {  L"New Dowload",
                                            L"Orig Download",
                                            L"Log File Path (Assumed Same as Copy To Path)" };
        std::vector<int> results(2);                                    
        
        // print blank lines
        for ( int i=0; i< 25; i++) std::wcout << L"\n";

        //hard code the new files path
        std::wcout << L"Place your New Files in c:\\t\\new\n";
        std::wcout << L"\n";

        this->copy_from_path = L"c:\\t\\new";
        results[1] = UserInput(msgs[1], this->copy_to_path);

        bool chk_results = true;    // assume results are good unless reset to false
        for (auto& result : results) {
            if (result != 0) chk_results = false;   // means that an error was encountered if not 0
        }
       
        if (chk_results) {  // prints the path info entered if chk_results is true
            print_path_info(msgs[0], this->copy_from_path);
            print_path_info(msgs[1], this->copy_to_path);
        } else {
            return -1;
        }
        // return standard 0 if no errors were encountered
        std::cout << std::endl;
        return 0;
    }

private:
    void print_path_info(std::wstring& msg, std::wstring& tmp_path) {
        std::wcout  << L"Path Set for '" 
                    << msg
                    << L"' was '"
                    << tmp_path
                    << L"' and was validated\n";
    }

    int UserInput (std::wstring msg_to_user, std::wstring& tmp_path) {
        // get user entered path
        std::wcout   << L"Enter the path to "
                    << msg_to_user << "\n"
                    << L"   Example 'c:\\t\\orig'\n"
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
        return 0;
    }
};


