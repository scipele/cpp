#pragma once

#include <unordered_map>
#include <string>
#include <filesystem>
#include "FileHasher.hpp"

class DirectoryAnalyzer {
private:
    std::wstring directory_path;
    FileHasher hasher;
    std::unordered_map<std::wstring, std::string> file_hashes;

    void analyzeDirectory() {
        std::size_t file_count = std::distance(
            std::filesystem::recursive_directory_iterator(directory_path),
            std::filesystem::recursive_directory_iterator{});
        
        file_hashes.reserve(file_count);  // Reserve space for the estimated number of files

        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory_path)) {
            if (std::filesystem::is_regular_file(entry)) {
                file_hashes[entry.path().wstring()] = hasher.sha1FileHash(entry.path());
            }
        }
    }

public:
    DirectoryAnalyzer(const std::wstring& path) : directory_path(path) {
        analyzeDirectory();  // Directly call the inline function here
    }

    const std::unordered_map<std::wstring, std::string>& getFileHashes() const {
        return file_hashes;
    }
};
