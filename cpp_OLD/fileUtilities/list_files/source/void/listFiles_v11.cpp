/*
listFiles_v11
Purpose:   Create a file listing with sha1 hash for a pathname entered by the user
           SHA-1 Hash creates a 40 Hexidecimal Digit unique id for the binary content of any file
Output:    The file listing is placed in the c:\t\file_list.csv which can be imported into excel and separated with pipe "|" either during import
Author:    Tony Sciple
Version:   v10
Date:      06/10/2024 */

#include <iostream>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstdint>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <codecvt>


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

    static std::wstring string_to_wstring(const std::string &str) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes(str);
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

    std::wstring sha1FileHash() const
    {
        // Open the file in binary mode
        std::ifstream file(it->path(), std::ios::binary);
        if (!file) {
            return L"";  // Return empty string to indicate error
        }

        // Initialize variables
        uint32_t h0 = 0x67452301;
        uint32_t h1 = 0xEFCDAB89;
        uint32_t h2 = 0x98BADCFE;
        uint32_t h3 = 0x10325476;
        uint32_t h4 = 0xC3D2E1F0;

        // Process each 512-bit chunk
        while (!file.eof()) {
            char buffer[64];
            file.read(buffer, 64);

            // Pad the last chunk if necessary
            if (file.gcount() < 64) {
                buffer[file.gcount()] = 0x80;
                for (int i = file.gcount() + 1; i < 64; ++i) {
                    buffer[i] = 0x00;
                }
                if (file.gcount() <= 56) {
                    uint64_t ml = (file.tellg() - file.gcount()) * 8;
                    for (int i = 63; i >= 56; --i) {
                        buffer[i] = (ml >> ((63 - i) * 8)) & 0xFF;
                    }
                }
            }

            uint32_t w[80];
            for (int j = 0; j < 16; ++j) {
                w[j] = (buffer[j * 4] << 24) | (buffer[j * 4 + 1] << 16) | (buffer[j * 4 + 2] << 8) | (buffer[j * 4 + 3]);
            }
            for (int j = 16; j < 80; ++j) {
                w[j] = (w[j - 3] ^ w[j - 8] ^ w[j - 14] ^ w[j - 16]);
                w[j] = (w[j] << 1) | (w[j] >> 31); // Left-rotate by 1
            }

            uint32_t a = h0;
            uint32_t b = h1;
            uint32_t c = h2;
            uint32_t d = h3;
            uint32_t e = h4;

            // Main loop
            for (int j = 0; j < 80; ++j) {
                uint32_t f, k;
                if (j < 20) {
                    f = (b & c) | ((~b) & d);
                    k = 0x5A827999;
                }
                else if (j < 40) {
                    f = b ^ c ^ d;
                    k = 0x6ED9EBA1;
                }
                else if (j < 60) {
                    f = (b & c) | (b & d) | (c & d);
                    k = 0x8F1BBCDC;
                }
                else {
                    f = b ^ c ^ d;
                    k = 0xCA62C1D6;
                }

                uint32_t temp = ((a << 5) | (a >> 27)) + f + e + k + w[j];
                e = d;
                d = c;
                c = ((b << 30) | (b >> 2));
                b = a;
                a = temp;
            }

            // Update hash values
            h0 += a;
            h1 += b;
            h2 += c;
            h3 += d;
            h4 += e;
        }

        // Convert hash components to hexadecimal string
        std::stringstream hash_ss;
        hash_ss << std::hex << std::setfill('0');
        for (uint32_t i : {h0, h1, h2, h3, h4}) {
            hash_ss << std::setw(8) << i;
        }

        return string_to_wstring(hash_ss.str());
    }


private:
    // Private default constructor for end iterator
    safe_dir_iter() : it(), ec(dummy_ec) {}

    static std::error_code dummy_ec;
};

// Initialize static member variable
std::error_code safe_dir_iter::dummy_ec;

// Function Declarations
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

    // Display only the error count
    std::cout << "\n\nFile information has been written to " << outputFile << "\n"
              << "for " << totalFiles << " files" << "\n";
    
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
    outFile << "row|parent_path|file_name|file_Size|sha1_hash|date_time|count" << "\n";
    
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

            if(currentFile == 1144) {
                std::cout << " temp pause";
            }


            outFile << currentFile << L"|" 
                    << dir.get_parent_path() << L"|" 
                    << dir.get_filename() << L"|"
                    << dir.get_file_size() << L"|"
                    << dir.sha1FileHash() << L"|" 
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