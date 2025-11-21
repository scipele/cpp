#include <iostream>
#include <filesystem>
#include <string>

int main(int argc, char const *argv[])
{
    // Prompt for directory
    std::wcout << L"Enter the path to rename the files: ";
    std::wstring directory;
    std::getline(std::wcin, directory);

    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file() && !entry.is_symlink()) {
            std::wstring orig_filename = entry.path().filename().wstring();

            // determine if filename already ends with ".pdf" (case-insensitive)
            bool has_pdf = false;
            auto pos = orig_filename.find_last_of(L'.');
            if (pos != std::wstring::npos) {
                std::wstring ext = orig_filename.substr(pos);
                for (auto &ch : ext) ch = std::tolower(ch);
                if (ext == L".pdf") has_pdf = true;
            }

            if (!has_pdf) {
                // replace existing extension with .pdf, or append if none
                std::wstring new_filename;
                if (pos != std::wstring::npos) {
                    new_filename = orig_filename.substr(0, pos) + L".pdf";
                } else {
                    new_filename = orig_filename + L".pdf";
                }
                try
                {
                    std::filesystem::rename(entry.path(), entry.path().parent_path() / new_filename);
                    std::wcout << L"Renamed: " << orig_filename << L" -> " << new_filename << L"\n";
                }
                catch(const std::exception& e) {
                    std::wcerr << L"Error renaming " << orig_filename << L": " << e.what() << L"\n";
                }
            }
        }
    }
    
    system("pause");
    return 0;
}
