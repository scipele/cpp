//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | copy_files_in_list.cpp                                      |
//| EntryPoint   | main                                                        |
//| Purpose      | copy files  listed in file_path_names.csv to given path     |
//| Inputs       | user entered path                                           |
//| Outputs      | file copy operation                                         |
//| Dependencies | none                                                        |
//| By Name,Date | T.Sciple, 9/4/2025                                          |

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>


std::vector<std::wstring> readFilePaths(const std::string& csvFilePath);
std::wstring get_filename(const std::wstring& path);


int main() {

    std::string csvFilePath = "file_path_names.csv";
    std::vector<std::wstring> paths = readFilePaths(csvFilePath);

    std::cout << "Enter destination path to copy files: ";
    
    std::wstring copy_to_path;
    std::getline(std::wcin, copy_to_path);

     if (!std::filesystem::exists(copy_to_path)) {
        std::cerr << "Error: The specified path does not exist." << std::endl;
        system("pause");
        return -1; // Exit with error
     }

    // Copy the files and print list to screen 
    int indx=1;
    int copied_count=0;

    for (const auto& path : paths) {
        std::wstring file_name_no_path = get_filename(path);
        std::wstring dest_path = copy_to_path + L"\\" + file_name_no_path;
        std::wcout << L"|" << path << std::endl;
        try {
            std::filesystem::copy(path, dest_path, std::filesystem::copy_options::overwrite_existing);
            std::wcout << L"copied " << indx << L"|" << path << std::endl;
            copied_count++;
        } catch (const std::filesystem::filesystem_error& e) { 
            std::wcout << indx << L"copy error->" << path << std::endl;
        }
        indx++;
    }

    std::wcout << L"Copied " << copied_count << L" of " << indx-1 << L" files." <<std::endl;
    system("pause");
    return 0;
}


std::wstring get_filename(const std::wstring& path) {
    size_t pos = path.find_last_of(L"/\\");
    return pos == std::wstring::npos ? path : path.substr(pos + 1);
}


std::vector<std::wstring> readFilePaths(const std::string& csvFilePath) {
    std::vector<std::wstring> filePaths;
    
    // Open the CSV file
    std::wifstream file(csvFilePath);
    if (!file.is_open()) {
        std::wcerr << L"Error: Could not open file " << std::wstring(csvFilePath.begin(), csvFilePath.end()) << std::endl;
        return filePaths;
    }

    // Count lines to reserve vector capacity
    std::wstring line;
    std::size_t lineCount = 0;
    while (std::getline(file, line)) {
        ++lineCount;
    }
    
    // Reserve space in vector
    filePaths.reserve(lineCount);
    
    // Reset file to beginning
    file.clear();
    file.seekg(0);

    // Read file paths
    while (std::getline(file, line)) {
        // Remove any trailing carriage return (for Windows compatibility)
        if (!line.empty() && line.back() == L'\r') {
            line.pop_back();
        }
        filePaths.push_back(line);
    }

    file.close();
    return filePaths;
}