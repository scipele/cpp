//| Item	     | Documentation Notes                                         |
//|--------------|-------------------------------------------------------------|
//| Filename     | list_file_path_names.cpp                                    |
//| EntryPoint   | main                                                        |
//| Purpose      | recursively list files in specified path                    |
//| Inputs       | user entered path                                           |
//| Outputs      | file listing                                                |
//| Dependencies | see std includes                                            |
//| By Name,Date | T.Sciple, 10/17/2025                                        |

#include <filesystem>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <locale>
#include <fstream>

// Function Prototype
size_t file_list(const std::filesystem::path& directory, bool include_subfolders);

int main() {
    std::wstring directory;
    std::cout << "Enter the path to list the files: ";
    std::getline(std::wcin, directory);

    int include_subfolders;
    std::cout << "Enter '1' to include files in subfolders, otherwise enter '0': ";
    std::cin >> include_subfolders;
    std::cout << std::endl;

    size_t cnt = file_list(directory, include_subfolders);
    std::cout << "See file listing in path provided named '_file_list.csv'\n";
    std::cout << "File Count: " << cnt << std::endl;
    system("pause");

    return 0;
}

size_t file_list(const std::filesystem::path& directory, bool include_subfolders) {
    size_t file_count = 0;
    const std::wstring output_filename = L"_file_list.csv"; // Define output file name

    // Open output file in the specified directory
    std::filesystem::path output_path = directory / output_filename;
    std::wofstream out_file(output_path, std::ios::out | std::ios::trunc);
    if (!out_file.is_open()) {
        std::wcerr << L"Error: Could not open output file: " << output_path.wstring() << L"\n";
        return 0;
    }

    out_file << L"id|path_str|filename|last_mod_date\n";

    auto print_entry = [&out_file](const std::filesystem::directory_entry& entry, size_t file_count) {
        // Get filename
        std::wstring path_str = entry.path().parent_path().wstring();
        std::wstring filename = entry.path().filename().wstring();

        // Get last modification time
        auto ftime = entry.last_write_time();
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
        std::time_t ctime = std::chrono::system_clock::to_time_t(sctp);

        // Format time to string with safe locale
        std::wstringstream wss;
        try {
            wss.imbue(std::locale("C"));
        } catch (const std::runtime_error&) {
            // Fallback: Do nothing (default locale)
        }
        wss << std::put_time(std::localtime(&ctime), L"%Y-%m-%d");
        std::wstring time_str = wss.str();

        // Write to file
        out_file << file_count << L"|" << path_str << L"|" << filename << L"|" << time_str << L"\n";
    };

    try {
        if (include_subfolders) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
                if (entry.is_regular_file() && !entry.is_symlink() && 
                    entry.path().filename().wstring() != output_filename) { // Skip output file
                    print_entry(entry, file_count);
                    ++file_count;
                }
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file() && !entry.is_symlink() && 
                    entry.path().filename().wstring() != output_filename) { // Skip output file
                    print_entry(entry, file_count);
                    ++file_count;
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::wcerr << L"Error accessing directory: " << e.what() << L"\n";
        out_file.close();
        return 0;
    }

    out_file.close();
    return file_count;
}