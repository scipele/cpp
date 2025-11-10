#include "../include/FileHasher.hpp"
#include <iostream>
#include <array>

int main() {
    // Initialize FileHasher with default settings
    FileHasher hasher;

    std::array<std::wstring, 4> filePaths = {
        L"C:/t/test/60-PKDS-Z3303_a.xlsx",
        L"C:/t/test/60-PKDS-Z3303_b.xlsx",
        L"C:/t/test/60-PKDS-Z3303_a.fake",
        L"C:/t/test/60-PKDS-Z3303_b.fake"
    };

    // Hash each file
    for (const auto& file_path : filePaths) {
        try {
            std::string hash = hasher.hashFile(file_path);
            
            std::wcout << "SHA-1 hash for " << file_path << ": " << hash.c_str() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    // Pause the console window before exiting
    system("pause");
    return 0;
}