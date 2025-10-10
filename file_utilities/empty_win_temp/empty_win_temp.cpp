#include <windows.h>
#include <iostream>
#include <string>

bool IsFileLocked(const std::wstring& filePath) {
    // Attempt to open the file with write access to check if it's locked
    HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        // If file is in use (ERROR_SHARING_VIOLATION) or access denied, consider it locked
        DWORD error = GetLastError();
        return (error == ERROR_SHARING_VIOLATION || error == ERROR_ACCESS_DENIED);
    }
    CloseHandle(hFile);
    return false;
}

bool DeleteUnlockedFiles(const std::wstring& directory) {
    std::wstring searchPath = directory + L"\\*.*";
    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Failed to access directory: " << directory << L". Error: " << GetLastError() << std::endl;
        return false;
    }

    bool success = true;
    do {
        // Skip directories (including "." and "..")
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }

        std::wstring filePath = directory + L"\\" + findData.cFileName;

        // Check if the file is locked
        if (!IsFileLocked(filePath)) {
            // Attempt to delete the file
            if (DeleteFileW(filePath.c_str())) {
                std::wcout << L"Deleted: " << filePath << std::endl;
            } else {
                std::wcerr << L"Failed to delete: " << filePath << L". Error: " << GetLastError() << std::endl;
                success = false;
            }
        } else {
            std::wcout << L"Skipped (locked): " << filePath << std::endl;
        }
    } while (FindNextFileW(hFind, &findData));

    FindClose(hFind);
    return success;
}

int main() {
    // Specify the target directory
    std::wstring targetDir = L"C:\\Users\\tsciple\\AppData\\Local\\Temp";

    std::wcout << L"Scanning directory: " << targetDir << std::endl;

    if (!DeleteUnlockedFiles(targetDir)) {
        std::wcerr << L"Some files could not be deleted." << std::endl;
        return 1;
    }

    std::wcout << L"Operation completed." << std::endl;
    return 0;
}