// ************ MAIN PROGRAM ***************************************************
//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | backupEstimLog.cpp                                          |
//| EntryPoint   | main                                                        |
//| Purpose      | copy estimate log backend                                   |
//| Inputs       | none                                                        |
//| Outputs      | creates a copy of the file                                  |
//| Dependencies | none                                                        |
//| By Name,Date | T.Sciple, 09/03/2025                                        |

#include <chrono>
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <windows.h>

// Function to get current date and time as a string suitable for filenames
std::string getCurrentDateTimeString() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&currentTime);

    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y%m%d_%H%M%S");
    return oss.str();
}

// Function to copy a file using Windows API
bool copyFile(const std::string& sourcePath, const std::string& destinationPath) {
    // Convert std::string to wide string for Windows API
    std::wstring wSourcePath(sourcePath.begin(), sourcePath.end());
    std::wstring wDestinationPath(destinationPath.begin(), destinationPath.end());

    // Open the source file with sharing enabled
    HANDLE hSource = CreateFileW(
        wSourcePath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE, // Allow other processes to read/write
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hSource == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: Could not open source file: " << sourcePath
                  << " (Error code: " << GetLastError() << ")" << std::endl;
        return false;
    }

    // Create the destination file
    HANDLE hDestination = CreateFileW(
        wDestinationPath.c_str(),
        GENERIC_WRITE,
        0, // Exclusive access for destination
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hDestination == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: Could not create destination file: " << destinationPath
                  << " (Error code: " << GetLastError() << ")" << std::endl;
        CloseHandle(hSource);
        return false;
    }

    // Buffer for file copy
    const DWORD bufferSize = 4096;
    char buffer[bufferSize];
    DWORD bytesRead, bytesWritten;

    // Copy file contents
    bool success = true;
    while (ReadFile(hSource, buffer, bufferSize, &bytesRead, NULL) && bytesRead > 0) {
        if (!WriteFile(hDestination, buffer, bytesRead, &bytesWritten, NULL) || bytesWritten != bytesRead) {
            std::cerr << "Error: Failed to write to destination file (Error code: " << GetLastError() << ")" << std::endl;
            success = false;
            break;
        }
    }

    if (!success || GetLastError() != ERROR_SUCCESS) {
        std::cerr << "Error: File copy failed (Error code: " << GetLastError() << ")" << std::endl;
    }

    // Clean up
    CloseHandle(hSource);
    CloseHandle(hDestination);

    return success;
}

int main() {
    std::string sourceFilePath = "\\\\RDSFS2\\Utils\\RCI\\Estimating\\EstimLog\\EstimateLog_be_v111.accdb";
    std::string backupDirectory = "\\\\RDSFS2\\Utils\\RCI\\Estimating\\EstimLog\\Backup\\";

    // Extract filename and extension
    size_t lastSlashPos = sourceFilePath.find_last_of("\\/");
    std::string filename = (lastSlashPos == std::string::npos) ? sourceFilePath : sourceFilePath.substr(lastSlashPos + 1);

    size_t dotPos = filename.find_last_of('.');
    std::string baseFilename = (dotPos == std::string::npos) ? filename : filename.substr(0, dotPos);
    std::string fileExtension = (dotPos == std::string::npos) ? "" : filename.substr(dotPos);

    // Construct the new destination filename with date and time
    std::string dateTimeString = getCurrentDateTimeString();
    std::string destinationFilename = baseFilename + "_" + dateTimeString + fileExtension;
    std::string destinationPath = backupDirectory + destinationFilename;

    if (copyFile(sourceFilePath, destinationPath)) {
        std::cout << "File copied successfully to: " << destinationPath << std::endl;
    } else {
        std::cerr << "File copy failed." << std::endl;
        return 1;
    }

    return 0;
}