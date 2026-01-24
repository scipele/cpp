/*
 * extract_pg1.cpp
 * Extracts the first page from all PDF files in an input folder
 * and saves them to an output folder.
 * 
 * Uses poppler-utils (pdfseparate) via system calls.
 */

#include <filesystem>
#include <string>
#include <iostream>
#include <cstdlib>

namespace fs = std::filesystem;

// Escape shell argument to handle spaces and special characters
std::string escapeShellArg(const std::string& arg) {
    std::string escaped = "'";
    for (char c : arg) {
        if (c == '\'') {
            escaped += "'\\''";
        } else {
            escaped += c;
        }
    }
    escaped += "'";
    return escaped;
}

bool extractFirstPage(const std::string& inputPath, const std::string& outputPath) {
    // Build command: pdfseparate -f 1 -l 1 input.pdf output.pdf
    // -f 1 = first page, -l 1 = last page (so only page 1)
    std::string cmd = "pdfseparate -f 1 -l 1 " + 
                      escapeShellArg(inputPath) + " " + 
                      escapeShellArg(outputPath);
    
    int result = std::system(cmd.c_str());
    
    if (result == 0) {
        std::cout << "Extracted: " << inputPath << " -> " << outputPath << std::endl;
        return true;
    } else {
        std::cerr << "Error extracting from: " << inputPath << std::endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_folder> <output_folder>" << std::endl;
        std::cerr << "  Extracts the first page from all PDFs in input_folder" << std::endl;
        std::cerr << "  and saves them to output_folder with '_page1' suffix." << std::endl;
        return 1;
    }

    std::string inputFolder = argv[1];
    std::string outputFolder = argv[2];

    // Verify input folder exists
    if (!fs::exists(inputFolder) || !fs::is_directory(inputFolder)) {
        std::cerr << "Error: Input folder does not exist: " << inputFolder << std::endl;
        return 1;
    }

    // Create output folder if it doesn't exist
    fs::create_directories(outputFolder);

    int successCount = 0;
    int failCount = 0;

    // Iterate through all files in the input folder
    for (const auto& entry : fs::directory_iterator(inputFolder)) {
        if (entry.is_regular_file() && entry.path().extension() == ".pdf") {
            std::string inputPath = entry.path().string();
            std::string outputFileName = entry.path().stem().string() + "_page1.pdf";
            std::string outputPath = (fs::path(outputFolder) / outputFileName).string();

            if (extractFirstPage(inputPath, outputPath)) {
                successCount++;
            } else {
                failCount++;
            }
        }
    }

    std::cout << "\nComplete: " << successCount << " succeeded, " << failCount << " failed." << std::endl;
    return (failCount > 0) ? 1 : 0;
}