//| Item	     | Documentation Notes                                         |
//|--------------|-------------------------------------------------------------|
//| Filename     | listFiles_wo_hash.cpp                                       |
//| EntryPoint   | main                                                        |
//| Purpose      | list files without hashing                                  |
//| Inputs       | by user                                                     |
//| Outputs      | saves to c:\t\filelist.csv                                  |
//| Dependencies | see std #includes                                           |
//| By Name,Date | T.Sciple, 12/16/2024                                        |

#include <array>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <cstdint>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <codecvt>
#include <vector>
#include <chrono>

// Defined a Class to return a Safe Directory Iterator
class safe_dir_iter
{
    std::filesystem::recursive_directory_iterator it;
    std::error_code &ec;

public:
    safe_dir_iter(const std::filesystem::path &path, std::error_code &ec) 
        : it(path, ec), ec(ec) {}

    safe_dir_iter &operator++() { 
        it.increment(ec); 
        return *this; 
    }

    const std::filesystem::directory_entry &operator*() const { 
        return *it; 
    }

    bool operator!=(const safe_dir_iter &other) const { 
        return it != other.it; 
    }

    static safe_dir_iter begin(const std::filesystem::path &path, std::error_code &ec) {
        return safe_dir_iter(path, ec);
    }

    static safe_dir_iter end_iter() {
        return safe_dir_iter();
    }

    std::wstring get_parent_path() const {
        return it->path().parent_path().wstring();
    }

    std::wstring get_filename() const {
        return it->path().filename().wstring();
    }

    std::uintmax_t get_file_size() const {
        return std::filesystem::file_size(it->path(), ec);
    }
    
    std::wstring get_full_path_wstr() const {
        return it->path().wstring();
    }

    std::wstring getLastWriteTime() const {
        HANDLE hFile = CreateFileW(it->path().wstring().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            // Error opening the file
            return L"Error";
        }

        FILETIME ftCreate, ftAccess, ftWrite;
        SYSTEMTIME stUTC, stLocal;

        // Retrieve the file times for the file.
        if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
        {
            CloseHandle(hFile);
            return FALSE;
        }

        // Convert the last-write time to local time.
        FileTimeToSystemTime(&ftWrite, &stUTC);
        SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

        // Build a string showing the date and time.
        WCHAR lpszString[256]; // Buffer to hold the formatted date and time
        StringCchPrintfW(lpszString, ARRAYSIZE(lpszString),
            L"%02d/%02d/%d  %02d:%02d",
        stLocal.wMonth, stLocal.wDay, stLocal.wYear,
        stLocal.wHour, stLocal.wMinute);

        CloseHandle(hFile);

    return std::wstring(lpszString); // Convert to std::wstring and return
    }
    
 
private:
    // Private default constructor for end iterator
    safe_dir_iter() : it(), ec(dummy_ec) {}

    static std::error_code dummy_ec;
};


// Initialize static member variable
std::error_code safe_dir_iter::dummy_ec;


// Function Prototypes
int listFilesRecursively(const std::filesystem::path& path, const std::string& outputFile, int& totalFiles);
void updateProgress(int current, int total);


int main() {

    // get user entered path
    std::wstring path;
    std::cout << "Enter the path to list files\n"
              << "   Example Entry 'c:\\myFolder'\n"
              << "   Or press 'c' to cancel\n"
              << "   User entry here ============> ";
    std::getline(std::wcin, path);

    // Allow user to cancel execution by entering 'c'
    if (path == L"c") {
        std::cout << "Execution cancelled by user." << std::endl;
        system("pause");
        return 0; // Exit gracefully
    }
    // Record start time
    auto start = std::chrono::high_resolution_clock::now();

    // Check if the entered path exists
    if (!std::filesystem::exists(path)) {
        std::cerr << "Error: The specified path does not exist." << std::endl;
        system("pause");
        return 1; // Exit with error code 1
    }

    // Define the output folder and filename
    const std::string outputFolderPath = "c:\\t";
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
    std::string outputFile = "c:\\t\\fileList.csv";

    int totalFiles = 0;
    listFilesRecursively(path, outputFile, totalFiles);

    // Display number of files processed
    std::cout << "\n\nFile information has been written to " << outputFile << "\n"
              << "for " << totalFiles << " files" << "\n";
    
        // Record end time
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration and output duration
    std::chrono::duration<double> duration = end - start;
    std::cout << "File Listing Compleded in : " << duration.count() << " seconds" << std::endl;
    
    // Pause the console window before exiting
    system("pause");

    return 0;
}


int getSafeFileCount(const std::filesystem::path& path) {
    // Get total file count for progress indication
    int fileCount = 0;
    std::error_code ec;
    for (auto dir = safe_dir_iter::begin(path, ec); dir != safe_dir_iter::end_iter(); ++dir) {
        if ((*dir).is_regular_file(ec)) {
            fileCount++;
        }
    }    
    return fileCount;
}


int listFilesRecursively(const std::filesystem::path& path, const std::string& outputFile, int& totalFiles) {
    std::wofstream outFile(outputFile);
    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open output file." << std::endl;
        return 1;
    }
    // place the header field names at the beginning of the file
    outFile << "row|parent_path|file_name|file_Size|date_time|count" << "\n";
    
    // Get total file count for progress indication
    totalFiles = getSafeFileCount(path);

    // Iterate over each entry in the directory recursively
    int currentFile = 0;
    int rowCount=0;
    std::error_code ec;
    for (auto dir = safe_dir_iter::begin(path, ec); dir != safe_dir_iter::end_iter(); ++dir) {
        if ((*dir).is_regular_file(ec)) {
            currentFile++;
            updateProgress(currentFile, totalFiles);
            std::cout.flush();

            outFile << currentFile << L"|" 
                    << dir.get_parent_path() << L"|" 
                    << dir.get_filename() << L"|"
                    << dir.get_file_size() << L"|"
                    << dir.getLastWriteTime() << L"|\n";
        }
        // troubleshooting only -> std::wcout << currentFile << L"." << dir.get_filename() << "\n";
    }
    return 0;
}


void updateProgress(int current, int total) {
    const int barWidth = 100;
    float progress = (float)current / total;
    int pos = barWidth * progress;

    std::cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << "%\r";
    std::cout.flush();
}