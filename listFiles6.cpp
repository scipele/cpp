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

//#include <chrono>

void listFilesRecursively(const std::filesystem::path& path, const std::string& outputFile);
BOOL GetLastWriteTime(const wchar_t* lpszFileName, LPTSTR lpszString, DWORD dwSize);
std::string sha1File(const std::string& filePath);
std::string hash_to_hex(std::string& hash);
std::wstring string_to_wstring(const std::string& str);


int main() {
    std::wstring path;
    std::cout << "Enter the path to list files: ";
    std::wcin >> path;

    // hard code where to place the filename
    std::string output_filename = "c:\\t\\file_list.csv";

    listFilesRecursively(path, output_filename);

    // Pause the console window before exiting
    system("pause");

    return 0;
}


void listFilesRecursively(const std::filesystem::path& path, const std::string& outputFile) {
    try {
        std::wofstream outFile(outputFile);
        if (!outFile.is_open()) {
            std::cerr << "Error: Unable to open output file." << std::endl;
            return;
        }
        outFile << "path | filename | fileSize | sha1hash | date_time" << "\n";

        // Iterate over each entry in the directory recursively
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            // Check if the entry is a regular file
            if (std::filesystem::is_regular_file(entry)) {
                // Get file size
                std::uintmax_t fileSize = std::filesystem::file_size(entry.path());
                // compute hash code for file
                std::string fpath_name = entry.path().string();
                // call sha-1 hash function
                std::string hash = sha1File(fpath_name);
                std::wstring hash_hx = string_to_wstring(hash_to_hex(hash));
                // Get last write time
                TCHAR date_time[MAX_PATH];
                GetLastWriteTime(entry.path().wstring().c_str(), date_time, MAX_PATH);
           //outFile << std::filesystem::absolute(entry.path()).wstring() << L"|" 

           
                // Use wcout instead of cout for wide characters
                outFile << std::filesystem::absolute(entry.path()).parent_path().wstring() << L"|" 
                        << entry.path().filename().wstring() << L"|" 
                        << fileSize << L"|" 
                        << hash_hx << L"|" 
                        << date_time << L"\n";
            }
        }
        std::cout << "File information has been written to " << outputFile << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

std::string sha1File(const std::string& filePath) {
    // Open the file in binary mode
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
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

    // Concatenate hash components
    std::string hash;
    for (uint32_t i : {h0, h1, h2, h3, h4}) {
        for (int j = 24; j >= 0; j -= 8) {
            hash += static_cast<char>((i >> j) & 0xFF);
        }
    }

    return hash;
}

std::string hash_to_hex(std::string& hash) {
    if (!hash.empty()) {
        // Convert each byte of the hash into its hexadecimal representation
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (unsigned char c : hash) {
            ss << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(c));
        }
        
        // Get the resulting string
        std::string hashString = ss.str();
        return hashString;
    }
    return "";

}

BOOL GetLastWriteTime(const wchar_t* lpszFileName, LPTSTR lpszString, DWORD dwSize)
{
    HANDLE hFile = CreateFileW(lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        // Error opening the file
        return FALSE;
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
    DWORD dwRet = StringCchPrintf(lpszString, dwSize,
        TEXT("%02d/%02d/%d  %02d:%02d"),
        stLocal.wMonth, stLocal.wDay, stLocal.wYear,
        stLocal.wHour, stLocal.wMinute);

    CloseHandle(hFile);

    return (S_OK == dwRet);
}

std::wstring string_to_wstring(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}
