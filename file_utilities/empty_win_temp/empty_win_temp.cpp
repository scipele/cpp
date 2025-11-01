//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | empty_win_temp.cpp                                          |
//| EntryPoint   | main                                                        |
//| Purpose      | fast cleanupcurrent user windows temp app data folder       |
//| Inputs       | none                                                        |
//| Outputs      | deleted files                                               |
//| Dependencies | standard only                                               |
//| By Name,Date | T.Sciple, 10/11/2025                                        |


#include <windows.h>
#include <iostream>
#include <string>

// Check if a file is locked by attempting to open it with write access
bool IsFileLocked(const std::wstring& filePath) {
    HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        return (error == ERROR_SHARING_VIOLATION || error == ERROR_ACCESS_DENIED);
    }
    CloseHandle(hFile);
    return false;
}

// Delete unlocked files and empty subdirectories in the specified directory recursively
bool DeleteUnlockedFiles(const std::wstring& directory, int& del_file_count, int& skipped_file_count, int& del_dir_count) {
    std::wstring searchPath = directory + L"\\*.*";
    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Failed to access directory: " << directory << L". Error: " << GetLastError() << std::endl;
        return false;
    }

    bool success = true;
    do {
        // Skip "." and ".." directories
        if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0) {
            continue;
        }

        std::wstring path = directory + L"\\" + findData.cFileName;

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // Recursively process subdirectory
            if (!DeleteUnlockedFiles(path, del_file_count, skipped_file_count, del_dir_count)) {
                success = false;
            }
            // Attempt to delete the directory (will succeed only if empty)
            if (RemoveDirectoryW(path.c_str())) {
                del_dir_count++;
                //std::wcout << L"Deleted directory: " << path << std::endl;
            } else {
                DWORD error = GetLastError();
                std::wcerr << L"Failed to delete directory: " << path << L". Error: " << error << std::endl;
                success = false;
            }
        } else {
            // Process file
            if (!IsFileLocked(path)) {
                if (DeleteFileW(path.c_str())) {
                    del_file_count++;
                    //std::wcout << L"Deleted: " << path << std::endl;
                } else {
                    DWORD error = GetLastError();
                    std::wcerr << L"Failed to delete: " << path << L". Error: " << error << std::endl;
                    success = false;
                }
            } else {
                skipped_file_count++;
                //std::wcout << L"Skipped (locked or access denied): " << path << std::endl;
            }
        }
    } while (FindNextFileW(hFind, &findData));

    FindClose(hFind);
    return success;
}

// Get the current user's temporary folder path
std::wstring GetUserTempPath() {
    wchar_t buffer[MAX_PATH];
    DWORD length = GetTempPathW(MAX_PATH, buffer);
    if (length == 0 || length > MAX_PATH) {
        std::wcerr << L"Failed to get temp path. Error: " << GetLastError() << std::endl;
        return std::wstring();
    }

    // Remove trailing backslash if present
    std::wstring tempPath(buffer);
    if (!tempPath.empty() && tempPath.back() == L'\\') {
        tempPath.pop_back();
    }

    return tempPath;
}

int main() {
    // Get the current user's temp directory
    std::wstring targetDir = GetUserTempPath();
    if (targetDir.empty()) {
        std::wcerr << L"Could not determine temp directory." << std::endl;
        return 1;
    }

    std::wcout << L"Scanning directory: " << targetDir << std::endl;

    int del_file_count = 0;
    int skipped_file_count = 0;
    int del_dir_count = 0;

    if (!DeleteUnlockedFiles(targetDir, del_file_count, skipped_file_count, del_dir_count)) {
        std::wcerr << L"Some files or directories could not be deleted." << std::endl;
    }

    std::wcout << L"Deleted files: " << del_file_count << L", Skipped locked files: " << skipped_file_count 
               << L", Deleted directories: " << del_dir_count << std::endl;
    std::wcout << L"Operation completed." << std::endl;

    system("pause");
    return 0;
}