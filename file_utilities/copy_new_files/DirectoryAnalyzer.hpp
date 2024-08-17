//filename:  DirectoryAnalyzer.hpp
#pragma once

#include <map>
#include <string>
#include "FileHasher.hpp"

class DirectoryAnalyzer {
    std::wstring directory_path;
    FileHasher hasher;

public:
    DirectoryAnalyzer(const std::wstring& path) : directory_path(path) {}

    std::map<std::wstring, std::wstring> get_hashes() {
        std::map<std::wstring, std::wstring> file_hashes;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory_path)) {
            if (std::filesystem::is_regular_file(entry)) {
                file_hashes[entry.path().wstring()] = hasher.sha1FileHash(entry.path());
            }
        }
        return file_hashes;
    }
};
