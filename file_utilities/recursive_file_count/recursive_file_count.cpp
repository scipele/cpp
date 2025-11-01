//| Item	     | Documentation Notes                                         |
//|--------------|-------------------------------------------------------------|
//| Filename     | recursive_file_count.cpp                                    |
//| EntryPoint   | main                                                        |
//| Purpose      | recursively count the files in specified path               |
//| Inputs       | hard coded path                                             |
//| Outputs      | count of files                                              |
//| Dependencies | see std includes                                            |
//| By Name,Date | T.Sciple, 1/12/2025                                         |

#include <filesystem>
#include <iostream>
#include <string>

// Function Prototype
size_t fileCount(const std::filesystem::path& directory);


int main() {
    std::wstring input_folder = L"c:/t/new";
    size_t cnt = fileCount(input_folder);
    std::cout << "File Count: " << cnt;
    return 0;
}


size_t fileCount(const std::filesystem::path& directory) {
    size_t file_count=0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file() && !entry.is_symlink()) {
            file_count ++;
        }
    }
    return file_count;
}