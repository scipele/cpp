#include "../../include/GetPaths.hpp"

int GetPaths::GetPathLocsAndValidate() {
    
    std::vector<std::wstring> msgs = { L"New Files Folder",
                                      L"Orig Files Folder",
                                      L"Log File Path (Assumed Same as Copy To Path)" };
    std::vector<int> results(2);                                    
    
    // print blank lines
    for ( int i=0; i< 25; i++) std::cout << "\n";

    //hard code the new files path
    this->copy_from_path = L"c:/t/new";
    this->copy_to_path = L"c:/t/orig";

    results[0] =IsPathValid(copy_from_path);
    results[1] =IsPathValid(copy_to_path);

    bool chk_results = true;    // assume results are good unless reset to false
    for (auto& result : results) {
        if (result != 0) chk_results = false;   // means that an error was encountered if not 0
    }
    
    if (chk_results) {  // prints the path info entered if chk_results is true
        PrintPathInfo(msgs[0], this->copy_from_path);
        PrintPathInfo(msgs[1], this->copy_to_path);
    } else {
        return -1;
    }
    // return standard 0 if no errors were encountered
    std::cout << std::endl;
    return 0;
}


void GetPaths::PrintPathInfo(std::wstring& msg, std::wstring& tmp_path) {
    std::wcout << msg
              << L"' in '"
              << tmp_path
              << L"' and was validated\n";
}


int GetPaths::IsPathValid(const std::wstring& tmp_path) {
    // Check if the entered path exists
    if (!std::filesystem::exists(tmp_path)) {
        std::cerr << "Error: The specified path does not exist." << std::endl;
        system("pause");
        return -1; // Exit with error
    }
    return 0;
}