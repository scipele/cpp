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
//| Windows Compile Using g++:
//      g++
//      -std=c++17
//      -I C:/poppler/include
//      -L C:/poppler/lib
//      -o ../bin/extr.exe
//      extr.cpp
//      -lpoppler-cpp
// g++ -std=c++17 -I C:/poppler/include -L C:/poppler/lib -o ../bin/extr.exe extr.cpp -lpoppler-cpp



#include <filesystem>
#include <string>
#include <iostream>
#include <memory>
#include <poppler-document.h>
#include <poppler-page.h>
#include <poppler-page-renderer.h>
#include <poppler-version.h>
#include <poppler-pdf-writer.h>

namespace fs = std::filesystem;


// Function declarations
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





// Extracts the first page of a PDF using Poppler C++ API
bool extractFirstPage(const fs::path& inputPath, const fs::path& outputPath) {
    std::unique_ptr<poppler::document> doc(poppler::document::load_from_file(inputPath.string()));
    if (!doc) {
        std::cerr << "Error: Could not open PDF: " << inputPath << std::endl;
        return false;
    }
    if (doc->is_locked()) {
        std::cerr << "Error: PDF is locked: " << inputPath << std::endl;
        return false;
    }
    if (doc->pages() < 1) {
        std::cerr << "Error: PDF has no pages: " << inputPath << std::endl;
        return false;
    }

    // Extract first page
    std::unique_ptr<poppler::page> page(doc->create_page(0));
    if (!page) {
        std::cerr << "Error: Could not extract first page from: " << inputPath << std::endl;
        return false;
    }

    // Write the single page to a new PDF
    poppler::pdf_writer writer(outputPath.string());
    writer.set_info("Title", page->label());
    writer.append_page(page.get());
    if (!writer.write()) {
        std::cerr << "Error: Could not write output PDF: " << outputPath << std::endl;
        return false;
    }

    std::cout << "Extracted: " << inputPath.string() << " -> " << outputPath.string() << std::endl;
    return true;
}