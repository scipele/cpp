#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>  // For std::filesystem::remove and path handling

// Function declarations
bool deleteFile(const std::wstring& fileName);
std::vector<std::wstring> read_file_names(const std::wstring& filename);

int main() {
    std::wstring directory;
    std::wcout << L"This program is used to delete files by name in a single path\n" << std::endl;
    std::wcout << L"Step 1:\nPlace a text file called '_del_files.txt' - in the path where the files are located:\n" << std::endl;
    std::wcout << L"Step 2:\nEnter the path to delete files: or 'c' to cancel: ";
    std::getline(std::wcin, directory);

    if (directory == L"c" || directory == L"C") {
        std::wcout << L"\nOperation cancelled by user." << std::endl;
        return 0;
    }

    // Ensure directory ends with a path separator (Windows-style)
    if (!directory.empty() && directory.back() != L'\\' && directory.back() != L'/') {
        directory += L'\\';
    }

    std::vector<std::wstring> files = read_file_names(directory + L"_del_files.txt");

    if (files.empty()) {
        std::wcout << L"No files to delete or file list is empty." << std::endl;
        return 0;
    }

    int deleted_file_count = 0;

    for (const auto& name : files) {
        std::wstring fullPath = directory + name;
        if (deleteFile(fullPath)) {
            deleted_file_count++;
        } else {
            std::wcout << L"Failed to delete: " << name << std::endl;
        }
    }

    std::wcout << L"Total files read   : " << files.size() << std::endl;
    std::wcout << L"Total files deleted: " << deleted_file_count << std::endl;

    // Pause the console window before exiting (Windows-specific)
    system("pause");

    return 0;
}

bool deleteFile(const std::wstring& fileName) {
    try {
        // std::filesystem::remove returns true if file existed and was deleted, false otherwise
        if (std::filesystem::remove(fileName)) {
            std::wcout << L"Successfully deleted: " << fileName << std::endl;
            return true;
        } else {
            std::wcerr << L"Error: File does not exist: " << fileName << std::endl;
            return false;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::wcerr << L"Error: Could not delete " << fileName << L" - " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::wstring> read_file_names(const std::wstring& filename) {
    std::vector<std::wstring> fileNames;
    // Convert wstring to filesystem path, then to string for wifstream
    std::filesystem::path filePath(filename);
    std::wifstream file(filePath.string()); // Use string() for wifstream

    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filePath.string() << std::endl;
        return fileNames;
    }

    std::wstring line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            fileNames.push_back(line);
        }
    }

    file.close();
    return fileNames;
}