#include <Windows.h>
#include <string>
#include <iostream>

int DeleteThumbsBBFiles(const std::wstring& directory, bool include_subfolders) {
    int files_deleted = 0;
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW((directory + L"\\*").c_str(), &findFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (wcscmp(findFileData.cFileName, L".") != 0 &&
                wcscmp(findFileData.cFileName, L"..") != 0) {
                std::wstring subDirectory = directory + L"\\" + findFileData.cFileName;
                if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    if (include_subfolders) {
                        files_deleted += DeleteThumbsBBFiles(subDirectory, include_subfolders);
                    }
                }
                else {
                    if (wcscmp(findFileData.cFileName, L"Thumbs.db") == 0) {
                        std::wstring filePath = directory + L"\\" + findFileData.cFileName;
                        if(DeleteFileW(filePath.c_str())) {
                            files_deleted++;
                        }
                        
                    }
                }
            }
        } while (FindNextFileW(hFind, &findFileData) != 0);
        FindClose(hFind);
    }
    return files_deleted;
}

int main() {
    std::wstring directory;
    std::cout << "Enter the path to delete the Thumbs.db files: ";
    std::getline(std::wcin, directory);

    int include_subfolders;
    std::cout << "Enter '1' to include deletion in subfolders, otherwise enter '0': ";
    std::cin >> include_subfolders;

    int files_deleted = DeleteThumbsBBFiles(directory, include_subfolders ==1);
    std::cout << "Total Thumbs.db files deleted: " << files_deleted << std::endl;

    // Pause the console window before exiting
    system("pause");
    
    return 0;
}
