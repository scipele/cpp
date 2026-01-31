//| Item	     | Class Documentation Notes                                   |
//|--------------|-------------------------------------------------------------|
//| Filename/type| GetPath.cpp / Class implementation  file                   |
//| EntryPoint   | instantiated from main                                      |
//| By Name,Date | T.Sciple, 1/30/2026                                         |

#include "../../include/GetPath.hpp"

// Constructor initialization with paths as empty strings
GetPathInfo::GetPathInfo()
    : list_files_path(L"") {
}

GetPathInfo::~GetPathInfo() {
    // Standard destructor
}

int GetPathInfo::GetPathLocsAndValidate() {
    
    std::vector<std::wstring> msgs = { L"List Files Folder",
                                      L"Log File Path (Assumed Same as Copy List Files Path)" };
    int result;                                    
    
    // print blank lines
    for ( int i=0; i< 25; i++) std::cout << "\n";

    //hard code the new files path
    
    std::wcout << L"Enter the folder path (or press Enter for default: C:\\t): ";
    std::wcout.flush();
    std::getline(std::wcin, this->list_files_path);
    if (this->list_files_path.empty()) {
        this->list_files_path = L"C:\\t";
        std::wcout << L"[DEBUG] Using default path: " << this->list_files_path << std::endl;
    } else {
        std::wcout << L"[DEBUG] User entered path: " << this->list_files_path << std::endl;
    }

    result = IsPathValid(list_files_path);

    bool chk_results = true;    // assume results are good unless reset to false
    if (result != 0) chk_results = false;   // means that an error was encountered if not 0
        
    if (chk_results) {  // prints the path info entered if chk_results is true
        PrintPathInfo(msgs[0], this->list_files_path);
    } else {
        return -1;
    }
    // return standard 0 if no errors were encountered
    std::cout << std::endl;
    return 0;
}


void GetPathInfo::PrintPathInfo(std::wstring& msg, std::wstring& tmp_path) {
    std::wcout << msg
              << L"' in '"
              << tmp_path
              << L"' and was validated\n";
}


int GetPathInfo::IsPathValid(const std::wstring& tmp_path) {
    // Check if the entered path exists
    std::wcout << L"[DEBUG] Checking path: '" << tmp_path << L"'" << std::endl;
    std::wcout.flush();
    bool exists = std::filesystem::exists(tmp_path);
    std::wcout << L"[DEBUG] std::filesystem::exists returned: " << (exists ? L"true" : L"false") << std::endl;
    std::wcout.flush();
    if (!exists) {
        std::cerr << "Error: The specified path does not exist." << std::endl;
        std::cerr << "[DEBUG] Path check failed for: ";
        std::wcerr << tmp_path << std::endl;
        std::cerr.flush();
        std::wcout.flush();
        system("pause");
        return -1; // Exit with error
    }
    return 0;
}


void GetPathInfo::printFileDataHeaderInfo() {
    std::cout   << "+-------------------------+---------------+-----------------+\n"
                << "| Path Location           |  File Counts  |  Folder Counts  |\n"
                << "+-------------------------+---------------+-----------------+\n";
}


void GetPathInfo::printFileAndFolderInfo(FilePropGatherer& OrigFiles) {

    // Use std::filesystem::path for safe conversion
    std::string pathA = std::filesystem::path(list_files_path).u8string();

    std::cout       << "| " 
                    << std::left << std::setw(24) << pathA << "|"
                    << std::right << std::setw(14) << FormatWithCommas(OrigFiles.fileCount) << " |"
                    << std::setw(16) << FormatWithCommas(OrigFiles.folderCount) << " |\n"
                << "+-------------------------+---------------+-----------------+" << std::endl;
}


std::string GetPathInfo::FormatWithCommas(size_t num) {
    std::string str = std::to_string(num);
    int bkw_cnt = str.length(); // initialize a backward counter to count down from string len to 0
    std::string formated_str;

    for (char c : str) {     // loops thru each character in the string
        formated_str += c;
        bkw_cnt--;  // deincrement the count by 1 after the first character which will always be there before checking for needed commas
        if (!bkw_cnt == 0) {
            if(bkw_cnt % 3 == 0) {  // checks if the mod of bkw_cnt / 3 == 0, or remainder is 0
                    formated_str += ',';
            }
        }
    }
    return formated_str;
}