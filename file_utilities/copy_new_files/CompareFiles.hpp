#pragma once
#include "DirectoryAnalyzer.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

class CompareFiles {
    std::wstring orig_path;
    std::wstring new_path;
    DirectoryAnalyzer orig_analyzer;
    DirectoryAnalyzer new_analyzer;

public:
    //Constructor
    CompareFiles(const std::wstring& orig_path, const std::wstring& new_path) 
        : orig_path(orig_path), new_path(new_path),
          orig_analyzer(orig_path), new_analyzer(new_path) {}

    void compare_and_copy() {
        // Create a map of original hash codes
        std::map<std::wstring, std::wstring> orig_hashes = orig_analyzer.get_hashes();

        // Get hashes for new files
        std::map<std::wstring, std::wstring> new_hashes = new_analyzer.get_hashes();

        // Create a set of hash codes from the original map for efficient lookup
        std::map<std::wstring, std::wstring> orig_hash_codes;
        // The auto keyword initializes a std::set from the orig_hashes map
        for (const auto& [file_path, file_hash] : orig_hashes) {
            orig_hash_codes[file_hash] = file_path; // Use file_hash as key
        }

        // Print the original and new file maps
        print_file_hashes(L"Original Files", orig_hashes);
        print_file_hashes(L"New Files", new_hashes);

        // Create a directory for added files with the current date
        std::wstring added_files_folder = orig_path + L"/added_files_" + get_current_date();
        std::filesystem::create_directory(added_files_folder);

        // Define the log file path inside the added_files_folder
        std::wstring log_file_path = added_files_folder + L"/file_comparison_log.csv";
       
        // Open the log file for recording operations
        std::wofstream log_file(log_file_path.c_str()); // Convert std::wstring to const wchar_t*
        if (!log_file) {
            std::wcerr << L"Error opening log file: " << log_file_path << std::endl;
            return;
        }

        // Iterate over the standard pairs from the new_hashes std::map and compare hashes
        //Create a heading in the log file
        log_file    << L"+------------------------------------------+-----------------------+--------------------------------------------------------------------------------+\n"
                    << L"| Hash Code                                | Remarks               | Filename                                                                       |\n"
                    << L"+------------------------------------------+-----------------------+--------------------------------------------------------------------------------+\n";

        for (const auto& [file_path, file_hash] : new_hashes) {
            if (orig_hash_codes.find(file_hash) == orig_hash_codes.end()) {
                // Hash code does not exist, so copy the file
                std::filesystem::path fs_path(file_path);

                // Create the relative path by removing new_path prefix from the file_path
                std::wstring relative_path = fs_path.lexically_relative(new_path).wstring();

                // Extract the filename and extension
                std::wstring filename = fs_path.stem().wstring();
                std::wstring extension = fs_path.extension().wstring();

                // Append the current date to the filename
                std::wstring new_filename = filename + L"_" + get_current_date() + extension;

                // Construct the destination paths within orig_path and added_files_folder, preserving the subfolder structure
                std::filesystem::path dest_orig_path = orig_path / fs_path.parent_path().lexically_relative(new_path) / new_filename;
                std::filesystem::path dest_added_files_path = added_files_folder / fs_path.parent_path().lexically_relative(new_path) / new_filename;

                // Ensure the destination directories exist
                std::filesystem::create_directories(std::filesystem::path(dest_orig_path).parent_path());
                std::filesystem::create_directories(std::filesystem::path(dest_added_files_path).parent_path());

                try {
                   
                    // Copy to the original path with subfolder structure
                    std::wcout << file_path << L"\n";
                    std::wcout << dest_orig_path << L"\n";

                    std::filesystem::copy(file_path, dest_orig_path, std::filesystem::copy_options::overwrite_existing);

                    // Copy to the added_files_folder with subfolder structure
                    std::filesystem::copy(file_path, dest_added_files_path, std::filesystem::copy_options::overwrite_existing);

                    log_file    << L"| " << std::left << std::setw(41) << file_hash
                                << L"| Copied new file       | " 
                                << std::setw(78) << file_path << L" |\n";

                } catch (const std::filesystem::filesystem_error& e) {
                    log_file    << L"| " << std::left << std::setw(41) << file_hash 
                                << L"| Copy Error            | "
                                   //| Remarks               |
                                << std::setw(78) << file_path << L" |\n";  // removed e.what()
                }
            } else {
                // Hash code already exists in the original hashes
                    log_file    << L"| " << std::left << std::setw(41) << file_hash 
                                << L"| Skip Exists in orig   | "
                                << std::setw(78) << file_path << L" |\n";
            }
        }
    }


private:
    std::map<std::wstring, std::wstring> get_original_hashes() {
        // Get the hash codes from the original directory
        auto orig_hashes = orig_analyzer.get_hashes();
        std::map<std::wstring, std::wstring> hash_map;

        for (const auto& [file_path, file_hash] : orig_hashes) {
            hash_map[file_path] = file_hash;
        }

        return hash_map;
    }

    void print_file_hashes(const std::wstring& title, const std::map<std::wstring, std::wstring>& file_hashes) {
        std::wcout << title << L":" << std::endl;
        std::wcout  << L"+------------------------------------------+-------------------------------------------------------------------------------+\n"
                    << L"| Hash Code                                |  Filename                                                                     |\n"
                    << L"+------------------------------------------+-------------------------------------------------------------------------------+\n";

        for (const auto& [file_path, file_hash] : file_hashes) {
            std::wcout << L"| " << std::left << std::setw(41) << file_hash << L"| " << std::setw(78) << file_path << L"|" << std::endl;
        }
        std::wcout << std::endl;
    }

    std::wstring get_current_date() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::wstringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), L"%Y_%m_%d");
        return ss.str();
    }
};
