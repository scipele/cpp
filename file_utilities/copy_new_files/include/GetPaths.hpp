//filename: GetPaths.hpp

#pragma once
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

class GetPaths {

public:
    std::wstring copy_from_path;
    std::wstring copy_to_path;
    int GetPathLocsAndValidate();

private:
    void PrintPathInfo(std::wstring& msg, std::wstring& tmp_path);
    int IsPathValid (const std::wstring& tmp_path);
};