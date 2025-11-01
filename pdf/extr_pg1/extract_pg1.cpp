#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include <filesystem>
#include <string>
#include <vector>
#include <iostream>

namespace fs = std::filesystem;

void extractFirstPage(const std::string& inputPath, const std::string& outputPath) {
    // Load the PDF document
    poppler::document* doc = poppler::document::load_from_file(inputPath);
    if (!doc) {
        std::cerr << "Error: Could not open PDF file: " << inputPath << std::endl;
        return;
    }

    // Check if document has at least one page
    if (doc->pages() < 1) {
        std::cerr << "Error: No pages in PDF file: " << inputPath << std::endl;
        delete doc;
        return;
    }

    // Create a new PDF document for the first page
    poppler::document* newDoc = new poppler::document(1);
    poppler::page* firstPage = doc->create_page(0); // 0-based index for first page

    // Copy the first page to the new document
    newDoc->copy_page(firstPage);

    // Save the new document
    if (!newDoc->save(outputPath)) {
        std::cerr << "Error: Could not save output PDF: " << outputPath << std::endl;
    } else {
        std::cout << "Saved first page of " << inputPath << " to " << outputPath << std::endl;
    }

    // Clean up
    delete firstPage;
    delete newDoc;
    delete doc;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_folder> <output_folder>" << std::endl;
        return 1;
    }

    std::string inputFolder = argv[1];
    std::string outputFolder = argv[2];

    // Create output folder if it doesn't exist
    fs::create_directories(outputFolder);

    // Iterate through all files in the input folder
    for (const auto& entry : fs::directory_iterator(inputFolder)) {
        if (entry.is_regular_file() && entry.path().extension() == ".pdf") {
            std::string inputPath = entry.path().string();
            std::string outputFileName = entry.path().stem().string() + "_page1.pdf";
            std::string outputPath = fs::path(outputFolder) / outputFileName;

            extractFirstPage(inputPath, outputPath);
        }
    }

    return 0;
}