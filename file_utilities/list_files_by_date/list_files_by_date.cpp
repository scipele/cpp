#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

// Global counters
unsigned long long totalFilesChecked = 0;
unsigned long long matchingFiles = 0;

// File stream for output
wofstream outputFile;

// Function to convert FILETIME to readable format for comparison
bool CompareFileTime(FILETIME ft, const SYSTEMTIME& targetDate) {
    try {
        FILETIME localFileTime;
        SYSTEMTIME sysTime;
        
        FileTimeToLocalFileTime(&ft, &localFileTime);
        FileTimeToSystemTime(&localFileTime, &sysTime);
        
        return (sysTime.wYear == targetDate.wYear &&
                sysTime.wMonth == targetDate.wMonth &&
                sysTime.wDay == targetDate.wDay);
    }
    catch (const exception& e) {
        outputFile << L"Error in CompareFileTime: " << e.what() << endl;
        return false;
    }
}

void SearchDirectory(const wstring& directory) {
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    
    SYSTEMTIME targetDate = {0};
    targetDate.wYear = 2025;
    targetDate.wMonth = 4;
    targetDate.wDay = 1;
    
    wstring searchPath = directory + L"\\*";
    
    try {
        hFind = FindFirstFileW(searchPath.c_str(), &findFileData);
        
        if (hFind == INVALID_HANDLE_VALUE) {
            outputFile << L"Error accessing directory: " << directory << endl;
            return;
        }
        
        do {
            if (wcscmp(findFileData.cFileName, L".") == 0 || 
                wcscmp(findFileData.cFileName, L"..") == 0) {
                continue;
            }
            
            wstring fullPath = directory + L"\\" + findFileData.cFileName;
            
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                SearchDirectory(fullPath);
            }
            else {
                totalFilesChecked++;
                
                bool matchCreate = CompareFileTime(findFileData.ftCreationTime, targetDate);
                bool matchModify = CompareFileTime(findFileData.ftLastWriteTime, targetDate);
                
                if (matchCreate || matchModify) {
                    matchingFiles++;
                    
                    FILETIME localTime;
                    SYSTEMTIME sysTime;
                    
                    FileTimeToLocalFileTime(&findFileData.ftLastWriteTime, &localTime);
                    FileTimeToSystemTime(&localTime, &sysTime);
                    
                    // Output only to file
                    outputFile << fullPath << L" - Last Modified: "
                              << setw(4) << sysTime.wYear << L"-"
                              << setw(2) << setfill(L'0') << sysTime.wMonth << L"-"
                              << setw(2) << setfill(L'0') << sysTime.wDay << L" "
                              << setw(2) << setfill(L'0') << sysTime.wHour << L":"
                              << setw(2) << setfill(L'0') << sysTime.wMinute << endl;
                }
            }
            
        } while (FindNextFileW(hFind, &findFileData) != 0);
        
        FindClose(hFind);
    }
    catch (const exception& e) {
        outputFile << L"Error in SearchDirectory: " << e.what() << endl;
        if (hFind != INVALID_HANDLE_VALUE) {
            FindClose(hFind);
        }
    }
}

int main() {
    try {
        // Open output file
        outputFile.open("search_results.txt", ios::out | ios::trunc);
        if (!outputFile.is_open()) {
            throw runtime_error("Could not create output file!");
        }
        
        outputFile << L"Searching D: drive for files created/modified on April 1, 2025...\n";
        outputFile << L"Search results for files created/modified on April 1, 2025:\n";
        outputFile << L"----------------------------------------------------\n";
        
        SearchDirectory(L"D:");
        
        // Write summary to file
        outputFile << L"\nSearch completed.\n";
        outputFile << L"Total files checked: " << totalFilesChecked << endl;
        outputFile << L"Matching files found: " << matchingFiles << endl;
        
        outputFile.close();
        
        return 0;
    }
    catch (const exception& e) {
        if (outputFile.is_open()) {
            outputFile << L"Error in main: " << e.what() << endl;
            outputFile.close();
        }
        return 1;
    }
}