//| Item	     | Documentation Notes                                         |
//|--------------|-------------------------------------------------------------|
//| Filename     | list_file_path_names.cpp                                    |
//| EntryPoint   | main                                                        |
//| Purpose      | recursively list files in specified path                    |
//| Inputs       | user entered path                                           |
//| Outputs      | file listing                                                |
//| Dependencies | see std includes                                            |
//| By Name,Date | T.Sciple, 10/17/2025                                        |
// Compile with : g++ -std=c++17 -o list_file_path_names list_file_path_names.cpp -static

#include <filesystem>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <locale>
#include <fstream>
#include <codecvt>
#include <locale>
#include <thread>
#include <chrono>
#include <vector>

// Function Prototype
size_t file_list(const std::filesystem::path& directory, bool include_subfolders);
void print_progress(size_t current, size_t total, size_t bar_width);


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


// Simple progress bar function
void print_progress(size_t current, size_t total, size_t bar_width = 50) {
    if (total > 0) {
        float progress = float(current) / total;
        size_t pos = static_cast<size_t>(bar_width * progress);
        std::cout << "[";
        for (size_t i = 0; i < bar_width; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << int(progress * 100.0) << "% (" << current << "/" << total << ")\r";
    } else {
        std::cout << "Files found: " << current << "\r";
    }
    std::cout.flush();
}

size_t file_list(const std::filesystem::path& directory, bool include_subfolders) {
    auto time_start = std::chrono::steady_clock::now();

    // Struct to hold file info
    struct FileInfo {
        std::filesystem::path path;
        std::filesystem::file_time_type last_write;
    };

    std::vector<FileInfo> files;
    size_t file_count = 0;
    const std::wstring output_filename = L"_file_list.csv"; // Define output file name


    // Helper: convert wstring to UTF-8 string
    auto to_utf8 = [](const std::wstring& wstr) -> std::string {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        return conv.to_bytes(wstr);
    };

    // Open output file in the specified directory (UTF-8)
    std::filesystem::path output_path = directory / output_filename;
    std::ofstream out_file(output_path, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!out_file.is_open()) {
        std::wcerr << L"Error: Could not open output file: " << output_path.wstring() << L"\n";
        return 0;
    }

    std::cout << "Scanning files in directory: " << to_utf8(directory.wstring()) << std::endl;

    out_file << "id|path_str|filename|last_mod_date\n";

    auto print_entry = [&out_file, &to_utf8](const std::filesystem::directory_entry& entry, size_t file_count) {
        // Get filename
        std::wstring path_str = entry.path().parent_path().wstring();
        std::wstring filename = entry.path().filename().wstring();

        // Check for long path or filename (Windows limit is 260, but streams may fail earlier)
        if (path_str.length() > 240 || filename.length() > 240) {
            std::wcerr << L"Warning: Skipping file with long path or filename: " << path_str << L"\\" << filename << L"\n";
            return;
        }

        // Sanitize output: replace newlines and pipes
        auto sanitize = [](const std::wstring& s) {
            std::wstring out = s;
            for (auto& ch : out) {
                if (ch == L'\n' || ch == L'\r' || ch == L'|') ch = L'_';
            }
            return out;
        };
        std::wstring safe_path = sanitize(path_str);
        std::wstring safe_filename = sanitize(filename);

        // Print debug info
        // std::wstring full_path = path_str + L"\\" + filename;
        // std::wcerr << L"Debug: About to write file #" << file_count << L": " << full_path << L" (length: " << full_path.length() << L")\n";

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

        // Write to file as UTF-8 with error checking
        std::string output_line = std::to_string(file_count) + "|" + to_utf8(safe_path) + "|" + to_utf8(safe_filename) + "|" + to_utf8(time_str) + "\n";
        // std::cerr << "Debug: Output line: " << output_line << std::endl;
        out_file << output_line;
        out_file.flush();
        if (!out_file) {
            std::wcerr << L"\nError: Failed to write to output file after file: " << safe_filename << L" in path: " << safe_path << L"\n";
            std::wcerr << L"Stream state: fail=" << out_file.fail() << L", bad=" << out_file.bad() << L"\n";
            throw std::runtime_error("Output file stream failed");
        }
    };


    // Second pass: collect file info
    file_count = 0;
    std::cout << "Scanning files in directory: " << to_utf8(directory.wstring()) << std::endl;
    try {
        if (include_subfolders) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
                if (entry.is_regular_file() && !entry.is_symlink() && entry.path().filename().wstring() != output_filename) {
                    files.push_back({entry.path(), entry.last_write_time()});
                    ++file_count;
                    if (file_count % 100 == 0) print_progress(file_count, 0); // Show count only
                }
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file() && !entry.is_symlink() && entry.path().filename().wstring() != output_filename) {
                    files.push_back({entry.path(), entry.last_write_time()});
                    ++file_count;
                    if (file_count % 100 == 0) print_progress(file_count, 0);
                }
            }
        }
        if (file_count > 0) std::cout << std::endl;
        std::cout.flush();
        std::cout << "Files found: " << file_count << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error accessing directory (scan phase): " << e.what() << std::endl;
        out_file.close();
        return 0;
    }

    // Write file info and show progress
    size_t total_files = files.size();
    size_t out_count = 0;
    std::cout << "Writing file information to output..." << std::endl;
    for (const auto& finfo : files) {
        try {
            // Get filename and path
            std::wstring path_str = finfo.path.parent_path().wstring();
            std::wstring filename = finfo.path.filename().wstring();
            // Check for long path or filename
            if (path_str.length() > 240 || filename.length() > 240) {
                std::cerr << "Warning: Skipping file with long path or filename: "
                          << to_utf8(path_str + L"\\" + filename) << std::endl;
                ++out_count;
                continue;
            }
            // Sanitize output
            auto sanitize = [](const std::wstring& s) {
                std::wstring out = s;
                for (auto& ch : out) {
                    if (ch == L'\n' || ch == L'\r' || ch == L'|') ch = L'_';
                }
                return out;
            };
            std::wstring safe_path = sanitize(path_str);
            std::wstring safe_filename = sanitize(filename);
            // Format time
            auto ftime = finfo.last_write;
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
            std::time_t ctime = std::chrono::system_clock::to_time_t(sctp);
            std::wstringstream wss;
            try { wss.imbue(std::locale("C")); } catch (...) {}
            wss << std::put_time(std::localtime(&ctime), L"%Y-%m-%d");
            std::wstring time_str = wss.str();
            // Write to file as UTF-8
            std::string output_line = std::to_string(out_count) + "|" + to_utf8(safe_path) + "|" + to_utf8(safe_filename) + "|" + to_utf8(time_str) + "\n";
            out_file << output_line;
        } catch (const std::exception& ex) {
            std::cerr << "Exception in output: " << ex.what() << std::endl;
        }
        ++out_count;
        if (total_files > 0 && (out_count % 100 == 0 || out_count == total_files)) print_progress(out_count, total_files);
    }
    if (total_files > 0) std::cout << std::endl;

    auto time_end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = time_end - time_start;
    std::cout << "Total time elapsed: " << std::fixed << std::setprecision(2) << elapsed.count() << " seconds." << std::endl;

    out_file.close();
    return file_count;
}