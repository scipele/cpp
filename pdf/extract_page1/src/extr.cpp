//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | extract_pg1.cpp                                             |
//| EntryPoint   | main                                                        |
//| Purpose      | strip the forst pdf sheet from all files in folder          |
//| Inputs       | folder containing the PDF files that you want to extract    |
//|              | the first page from                                         |
//| Outputs      | the first page of all the pdf files                         |
//| Dependencies | poppler-utils (pdfseparate) via system calls                |
//| By Name,Date | T.Sciple, 01/24/2026        
// Windows Compile Using MSYS2 UCRT64 Shell:
// g++ -std=c++17 -mconsole -o ../bin/extr.exe extr.cpp $(pkg-config --cflags --libs poppler-cpp) -lstdc++

#include <filesystem>
#include <string>
#include <iostream>
#include <memory>

namespace fs = std::filesystem;

// Function prototypes
bool extractFirstPage(const fs::path& inputPath, const fs::path& outputPath);
void pauseConsole();


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

    pauseConsole();
    return (failCount > 0) ? 1 : 0;
}


// Extracts the first page of a PDF using Poppler C++ API
bool extractFirstPage(const fs::path& inputPath, const fs::path& outputPath) {
    // Use pdfseparate to extract the first page as a new PDF
    // pdfseparate -f 1 -l 1 input.pdf output_pg1.pdf
    std::string input = inputPath.string();
    std::string output = outputPath.string();

    // pdfseparate expects an output pattern, e.g., output_%d.pdf
    // We'll extract to a temp file and then rename it
    fs::path tempOutput = outputPath.parent_path() / (outputPath.stem().string() + "_%d.pdf");
    std::string tempOutputStr = tempOutput.string();

#ifdef _WIN32
    // On Windows, quote paths with spaces
    std::string cmd = "pdfseparate -f 1 -l 1 \"" + input + "\" \"" + tempOutputStr + "\"";
#else
    std::string cmd = "pdfseparate -f 1 -l 1 '" + input + "' '" + tempOutputStr + "'";
#endif

    int result = std::system(cmd.c_str());
    if (result != 0) {
        std::cerr << "Error: pdfseparate failed for " << input << std::endl;
        return false;
    }

    // The output will be tempOutput with %d replaced by 1
    fs::path actualOutput = tempOutput;
    std::string tempName = tempOutput.string();
    size_t pos = tempName.find("%d");
    if (pos != std::string::npos) {
        tempName.replace(pos, 2, "1");
    }
    actualOutput = tempName;

    // Rename to the desired output file name
    std::error_code ec;
    fs::rename(actualOutput, outputPath, ec);
    if (ec) {
        std::cerr << "Error: Could not rename output file: " << actualOutput << " to " << outputPath << std::endl;
        return false;
    }

    std::cout << "Extracted: " << input << " -> " << output << std::endl;
    return true;
}


// Pause console for user input
void pauseConsole() {
#ifdef _WIN32
    system("pause");
#else
    std::cout << "Press Enter to continue...";
    std::cin.get(); 
#endif
}