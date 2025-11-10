//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | list_files_by_date.cpp                                      |
//| EntryPoint   | main                                                        |
//| Purpose      | list files in path of a given date                          |
//| Inputs       | user enterred path and date                                 |
//| Outputs      | screen and text file 'search_results.txt'                   |
//| Dependencies | none                                                        |
//| By Name,Date | T.Sciple, 10/24/2025                                        |

#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>


// Global counters
unsigned long long totalFilesChecked = 0;
unsigned long long matchingFiles = 0;


// File stream for output
std::wofstream outputFile;


// Function Prototypes
bool parseDate(const std::wstring& dateStr, int& month, int& day, int& year);
bool CompareFileTime(FILETIME ft, const SYSTEMTIME& targetDate);
bool isValidDate(int month, int day, int year);
void SearchDirectory(const std::wstring& directory, const SYSTEMTIME& targetDate);
SYSTEMTIME getSystemTimeFromInput(int month, int day, int year);


int main() {
    // Prompt for directory
    std::wcout << L"Enter the path to list the files: ";
    std::wstring directory;
    std::getline(std::wcin, directory);
    
    // Prompt for date
    std::wcout << L"Enter the date to search for (mm/dd/yyyy): ";
    std::wstring dateInput;
    std::getline(std::wcin, dateInput);
    int month, day, year;

    // Parse and validate date
    if (!parseDate(dateInput, month, day, year)) {
        std::wcout << L"Invalid date format or values. Please use mm/dd/yyyy with valid dates." << std::endl;
        return 1;
    }

    // Convert to SYSTEMTIME
    SYSTEMTIME targetDate = getSystemTimeFromInput(month, day, year);

    std::wcout << L"See saved results in file search_results.txt in the current path where the program was executed\n\n";
    std::wcout << L"List of matching files:" << std::endl;

    try {
        // Open output file
        outputFile.open("search_results.txt", std::ios::out | std::ios::trunc);
        if (!outputFile.is_open()) {
            throw std::runtime_error("Could not create output file!");
        }

        // Update output message with user-provided date
        outputFile << L"Searching directory for files modified on "
                   << std::setw(2) << std::setfill(L'0') << month << L"/"
                   << std::setw(2) << std::setfill(L'0') << day << L"/"
                   << year << L"...\n";
        outputFile << L"Search results for files modified on "
                   << std::setw(2) << std::setfill(L'0') << month << L"/"
                   << std::setw(2) << std::setfill(L'0') << day << L"/"
                   << year << L":\n";
        outputFile << L"----------------------------------------------------\n";
        
        SearchDirectory(directory, targetDate);
        
        // Write summary to file
        outputFile << L"\nSearch completed.\n";
        outputFile << L"Total files checked: " << totalFilesChecked << std::endl;
        outputFile << L"Matching files found: " << matchingFiles << std::endl;
        
        outputFile.close();

        system("pause");
        return 0;
    }
    catch (const std::exception& e) {
        if (outputFile.is_open()) {
            outputFile << L"Error in main: " << e.what() << std::endl;
            outputFile.close();
        }
        std::wcout << L"Error: " << e.what() << std::endl;
        return 1;
    }
}


// Function to convert parsed date to SYSTEMTIME
SYSTEMTIME getSystemTimeFromInput(int month, int day, int year) {
    SYSTEMTIME targetDate = {0};
    targetDate.wYear = year;
    targetDate.wMonth = month;
    targetDate.wDay = day;
    return targetDate;
}


// Function to convert SYSTEMTIME to FILETIME for a specific date (UTC)
bool SystemTimeToFileTimeUTC(const SYSTEMTIME& sysTime, FILETIME& fileTime) {
    return SystemTimeToFileTime(&sysTime, &fileTime) != 0;
}


// Function to compare FILETIME with target date (in UTC)
bool CompareFileTime(FILETIME ft, const SYSTEMTIME& targetDate) {
    try {
        // Convert target date to FILETIME for start of day (00:00:00 UTC)
        SYSTEMTIME startOfDay = targetDate;
        startOfDay.wHour = 0;
        startOfDay.wMinute = 0;
        startOfDay.wSecond = 0;
        startOfDay.wMilliseconds = 0;
        FILETIME startFileTime;
        if (!SystemTimeToFileTimeUTC(startOfDay, startFileTime)) {
            outputFile << L"Error converting target date to FILETIME" << std::endl;
            return false;
        }

        // Convert target date to FILETIME for end of day (23:59:59.999 UTC)
        SYSTEMTIME endOfDay = targetDate;
        endOfDay.wHour = 23;
        endOfDay.wMinute = 59;
        endOfDay.wSecond = 59;
        endOfDay.wMilliseconds = 999;
        FILETIME endFileTime;
        if (!SystemTimeToFileTimeUTC(endOfDay, endFileTime)) {
            outputFile << L"Error converting end of day to FILETIME" << std::endl;
            return false;
        }

        // Compare file time with the target day range
        return (CompareFileTime(&ft, &startFileTime) >= 0 && CompareFileTime(&ft, &endFileTime) <= 0);
    }
    catch (const std::exception& e) {
        outputFile << L"Error in CompareFileTime: " << e.what() << std::endl;
        return false;
    }
}


// Function to validate date
bool isValidDate(int month, int day, int year) {
    if (month < 1 || month > 12) return false;
    if (year < 1900 || year > 9999) return false;

    // Check for valid day based on month (with leap year support)
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && day == 29) {
        bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (!isLeapYear) return false;
    } else if (day < 1 || day > daysInMonth[month - 1]) {
        return false;
    }

    return true;
}


// Function to parse mm/dd/yyyy date input
bool parseDate(const std::wstring& dateStr, int& month, int& day, int& year) {
    if (dateStr.length() != 10 || dateStr[2] != L'/' || dateStr[5] != L'/') {
        return false;
    }

    std::wstringstream ss(dateStr);
    wchar_t slash;
    if (!(ss >> month >> slash >> day >> slash >> year)) {
        return false;
    }

    return isValidDate(month, day, year);
}


void SearchDirectory(const std::wstring& directory, const SYSTEMTIME& targetDate) {
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    std::wstring searchPath = directory + L"\\*";
    
    try {
        hFind = FindFirstFileW(searchPath.c_str(), &findFileData);
        
        if (hFind == INVALID_HANDLE_VALUE) {
            outputFile << L"Error accessing directory: " << directory << std::endl;
            return;
        }
        
        do {
            if (wcscmp(findFileData.cFileName, L".") == 0 || 
                wcscmp(findFileData.cFileName, L"..") == 0) {
                continue;
            }
            
            std::wstring fullPath = directory + L"\\" + findFileData.cFileName;
            
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                SearchDirectory(fullPath, targetDate);
            }
            else {
                totalFilesChecked++;
                
                bool matchModify = CompareFileTime(findFileData.ftLastWriteTime, targetDate);
                
                if (matchModify) {
                    matchingFiles++;
                    
                    FILETIME localTime;
                    SYSTEMTIME sysTime;
                    
                    FileTimeToLocalFileTime(&findFileData.ftLastWriteTime, &localTime);
                    FileTimeToSystemTime(&localTime, &sysTime);

                    std::wcout << fullPath << L" - Last Modified: "
                               << std::setw(4) << sysTime.wYear << L"-"
                               << std::setw(2) << std::setfill(L'0') << sysTime.wMonth << L"-"
                               << std::setw(2) << std::setfill(L'0') << sysTime.wDay << L" "
                               << std::setw(2) << std::setfill(L'0') << sysTime.wHour << L":"
                               << std::setw(2) << std::setfill(L'0') << sysTime.wMinute << std::endl;

                    outputFile << fullPath << L" - Last Modified: "
                              << std::setw(4) << sysTime.wYear << L"-"
                              << std::setw(2) << std::setfill(L'0') << sysTime.wMonth << L"-"
                              << std::setw(2) << std::setfill(L'0') << sysTime.wDay << L" "
                              << std::setw(2) << std::setfill(L'0') << sysTime.wHour << L":"
                              << std::setw(2) << std::setfill(L'0') << sysTime.wMinute << std::endl;
                }
            }
            
        } while (FindNextFileW(hFind, &findFileData) != 0);
        
        FindClose(hFind);
    }
    catch (const std::exception& e) {
        outputFile << L"Error in SearchDirectory: " << e.what() << std::endl;
        if (hFind != INVALID_HANDLE_VALUE) {
            FindClose(hFind);
        }
    }
}