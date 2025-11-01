//| Item	     | Class Documentation Notes                                   |
//|--------------|-------------------------------------------------------------|
//| Filename/type| GetPath.cpp / Class implementation  file                   |
//| EntryPoint   | instantiated from main                                      |
//| By Name,Date | T.Sciple, 1/18/2025                                         |

#include "../../include/GetPth.hpp"

// Constructor initialization with path as empty strings
GetPth::GetPth() : list_path(L"") {
}

GetPth::~GetPth() {
    // Standard destructor
}

int GetPth::GetPathLocsAndValidate() {
    
    std::vector<std::wstring> msgs = { L"List files folder location",
                                      L"Log File Path (placed in c/t/list_files.csv)" };
    std::vector<int> results(2);                                    
    
    // print blank lines
    for ( int i=0; i< 25; i++) std::cout << "\n";

    std::cout << "Enter folder to list the files: ";
    std::getline(std::wcin, this->list_path);

    results[0] =IsPathValid(list_path);

    bool chk_results = true;    // assume results are good unless reset to false
    for (auto& result : results) {
        if (result != 0) chk_results = false;   // means that an error was encountered if not 0
    }
    
    if (chk_results) {  // prints the path info entered if chk_results is true
        PrintPathInfo(msgs[0], this->list_path);
    } else {
        return -1;
    }
    // return standard 0 if no errors were encountered
    std::cout << std::endl;
    return 0;
}


void GetPth::PrintPathInfo(std::wstring& msg, std::wstring& tmp_path) {
    std::wcout << msg
              << L"' in '"
              << tmp_path
              << L"' and was validated\n";
}


int GetPth::IsPathValid(const std::wstring& tmp_path) {
    // Check if the entered path exists
    if (!std::filesystem::exists(tmp_path)) {
        std::cerr << "Error: The specified path does not exist." << std::endl;
        system("pause");
        return -1; // Exit with error
    }
    return 0;
}


void GetPth::printFileDataHeaderInfo() {
    std::cout   << "+-------------------------+---------------+-----------------+\n"
                << "| Path Location           |  File Counts  |  Folder Counts  |\n"
                << "+-------------------------+---------------+-----------------+\n";
}


void GetPth::printFileAndFolderInfo(FilePropGatherer& OrigFiles) {

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string pathA = converter.to_bytes(list_path);

    std::cout       << "| " 
                    << std::left << std::setw(24) << pathA << "|"
                    << std::right << std::setw(14) << FormatWithCommas(OrigFiles.fileCount) << " |"
                    << std::setw(16) << FormatWithCommas(OrigFiles.folderCount) << " |\n"
                << "+-------------------------+---------------+-----------------+" << std::endl;
}


std::string GetPth::FormatWithCommas(size_t num) {
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