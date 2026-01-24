//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | extract_pg1.cpp                                             |
//| EntryPoint   | main                                                        |
//| Purpose      | strip the forst pdf sheet from all files in folder          |
//| Inputs       | folder containing the PDF files that you want to extract    |
//|              | the first page from                                         |
//| Outputs      | the first page of all the pdf files                         |
//| Dependencies | poppler-utils (pdfseparate) via system calls                |
//| By Name,Date | T.Sciple, 01/24/2026                                        |


#include <filesystem>
#include <string>
#include <iostream>
#include <cstdlib>

namespace fs = std::filesystem;

// Detect platform
#ifdef _WIN32
    #define IS_WINDOWS 1
#else
    #define IS_WINDOWS 0
#endif

// Function declarations
std::string escapeShellArg(const fs::path& arg);
bool extractFirstPage(const fs::path& inputPath, const fs::path& outputPath);


int main(int argc, char* argv[]) {

    // Get path to executable
    fs::path exePath = fs::absolute(argv[0]);
    fs::path binFolder = exePath.parent_path();
    fs::path inputFolder = binFolder / ".." / "inp";
    fs::path outputFolder = binFolder / ".." / "out";
    inputFolder = fs::weakly_canonical(inputFolder);
    outputFolder = fs::weakly_canonical(outputFolder);

    std::cout << "Input folder:  " << inputFolder.string() << std::endl;
    std::cout << "Output folder: " << outputFolder.string() << std::endl;

    // Verify input folder exists
    if (!fs::exists(inputFolder) || !fs::is_directory(inputFolder)) {
        std::cerr << "Error: Input folder does not exist: " << inputFolder.string() << std::endl;
        return 1;
    }

    // Create output folder if it doesn't exist
    fs::create_directories(outputFolder);

    int successCount = 0;
    int failCount = 0;

    // Iterate through all files in the input folder
    for (const auto& entry : fs::directory_iterator(inputFolder)) {
        if (entry.is_regular_file() && entry.path().extension() == ".pdf") {
            fs::path inputPath = entry.path();
            fs::path outputFileName = entry.path().stem();
            outputFileName += "_pg1.pdf";
            fs::path outputPath = outputFolder / outputFileName;

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


// Escape shell argument to handle spaces and special characters
// Uses filesystem::path to handle international characters properly
std::string escapeShellArg(const fs::path& arg) {
#if IS_WINDOWS
    // Windows: use double quotes, escape internal double quotes
    // Convert to string for command line
    std::string pathStr = arg.string();
    std::string escaped = "\"";
    for (char c : pathStr) {
        switch (c) {
            case '"':
            escaped += "\\\"";
            break;
            case '\\':
            escaped += "\\\\";
            break;
            default:
            escaped += c;
            break;
        }
    }
    escaped += "\"";
    return escaped;
#else
    // Linux/Unix: use single quotes, escape internal single quotes
    // u8string returns UTF-8 which is native on Linux
    std::string pathStr = arg.u8string();
    std::string escaped = "'";
    for (char c : pathStr) {
        switch (c)
        {
        case '\'':
            escaped += "'\\''";
            break;
        default:
            escaped += c;
            break;
        }
    }
    escaped += "'";
    return escaped;
#endif
}


bool extractFirstPage(const fs::path& inputPath, const fs::path& outputPath) {
    // Build command: pdfseparate -f 1 -l 1 input.pdf output.pdf
    // -f 1 = first page, -l 1 = last page (so only page 1)
    std::string cmd = "pdfseparate -f 1 -l 1 " + 
                      escapeShellArg(inputPath) + " " + 
                      escapeShellArg(outputPath);
    
    int result = std::system(cmd.c_str());
    
    if (result == 0) {
        std::cout << "Extracted: " << inputPath.string() << " -> " << outputPath.string() << std::endl;
        return true;
    } else {
        std::cerr << "Error extracting from: " << inputPath.string() << std::endl;
        return false;
    }
}