//| Item         | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | dwg_rev.cpp                                                 |
//| EntryPoint   | main                                                        |
//| Purpose      | Extract drawing number and revision from PDFs               |
//| Inputs       | inp folder containing PDF files                             |
//| Outputs      | CSV file with pipe-delimited drawing number and rev data    |
//| Dependencies | libpoppler-cpp                                              |
//| By Name,Date | T.Sciple, 01/24/2026                                        |
//|--------------|-------------------------------------------------------------|
//| Usage        | dwg_rev                                                     |
//| Defaults     | ./inp ./out/bom_output.csv 10.89 5.33 0.81 6.78             |
//| Crop Region  | Extracts only from BOM area (right side of 11x17 drawing)   |
//|--------------|-------------------------------------------------------------|
//| Compile Linux| g++ -std=c++17 ./dwg_rev.cpp -o ../bin/dwg_rev $(pkg-config --cflags --libs poppler-cpp)
//| Compile in MSYS2 UCRT64 Term-> g++ -std=c++17 -o ../bin/dwg_rev.exe dwg_rev.cpp $(pkg-config --cflags --libs poppler-cpp) -lstdc++


#include <filesystem>
#include <string>
#include <iostream>
#include <regex>
#include <memory>
#include <fstream>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-global.h>

namespace fs = std::filesystem;

// Function prototypes
std::string trim(const std::string& str);
std::string getRevNumberPattern(const std::string& revText);
std::string extractRegionText(const poppler::page& page, double x1, double y1, double x2, double y2);
std::string extractAfterPrefix(const std::string& text, const std::string& prefix);


int main(int argc, char* argv[]) {
        // auto printTextBoxCoords = [](poppler::page& page) {
        //     std::vector<poppler::text_box> boxes = page.text_list();
        //     std::cout << "\n--- DEBUG: Text box coordinates for 'FILE:', 'REV.', '0' ---\n";
        //     for (const auto& box : boxes) {
        //         auto utf8vec = box.text().to_utf8();
        //         std::string txt;
        //         if (!utf8vec.empty())
        //             txt.assign(utf8vec.begin(), utf8vec.end());
        //         else
        //             txt = "";
        //         if (txt.find("FILE:") != std::string::npos || txt.find("REV.") != std::string::npos || txt == "0") {
        //             auto rect = box.bbox();
        //             std::cout << "Text: '" << txt << "' at (x=" << rect.x() / 72.0 << ", y=" << rect.y() / 72.0 << ", w=" << rect.width() / 72.0 << ", h=" << rect.height() / 72.0 << ") inches\n";
        //         }
        //     }
        //     std::cout << "--- END DEBUG ---\n";
        // };
    // Always resolve inp folder relative to the source file location
    fs::path inputFolder = argc > 1 ? argv[1] : (fs::path(__FILE__).parent_path().parent_path() / "inp");
    if (!fs::exists(inputFolder) || !fs::is_directory(inputFolder)) {
        std::cerr << "Error: Input folder does not exist: " << inputFolder << std::endl;
        return 1;
    }

    // PDF region extraction using (x, y, width, height) in inches
    // FILE region: x=15.0, y=10.70, w=0.2, h=0.11
    // REV region:  x=15.95, y=10.2, w=0.3, h=0.375

    double file_x_in = 15.0, file_y_in = 10.70, file_w_in = 0.2, file_h_in = 0.11;
    double rev_x_in = 15.95, rev_y_in = 10.24, rev_w_in = 0.2, rev_h_in = 0.35;

    double file_x1 = file_x_in * 72, file_x2 = (file_x_in + file_w_in) * 72;
    double file_y1 = file_y_in * 72, file_y2 = (file_y_in + file_h_in) * 72;
    double rev_x1 = rev_x_in * 72, rev_x2 = (rev_x_in + rev_w_in) * 72;
    double rev_y1 = rev_y_in * 72, rev_y2 = (rev_y_in + rev_h_in) * 72;

    fs::path sourceDir = fs::path(__FILE__).parent_path();
    fs::path outputFile = sourceDir.parent_path() / "out/dwg_rev_output.csv";
    fs::create_directories(outputFile.parent_path());
    std::ofstream csv(outputFile);
    csv << "sep=|" << std::endl;
    csv << "fileNameNoParentPath|fileOnPdf|RevNo|pdfPage" << std::endl;

    for (const auto& entry : fs::directory_iterator(inputFolder)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".pdf") continue;
        std::string fname = entry.path().filename().string();
        std::unique_ptr<poppler::document> doc(poppler::document::load_from_file(entry.path().string()));
        if (!doc || doc->is_locked() || doc->pages() == 0) {
            std::cerr << "Could not open PDF: " << fname << std::endl;
            continue;
        }
        int numPages = doc->pages();
        for (int pageNum = 0; pageNum < numPages; ++pageNum) {
            std::unique_ptr<poppler::page> page(doc->create_page(pageNum));
            if (!page) continue;
            std::string fileText = extractRegionText(*page, file_x1, file_y1, file_x2, file_y2);
            std::string revText = extractRegionText(*page, rev_x1, rev_y1, rev_x2, rev_y2);
            std::string fileVal = extractAfterPrefix(fileText, "FILE:");
            std::string revVal = getRevNumberPattern(revText);
            csv << fname << "|" << (fileVal.empty() ? "(not found)" : fileVal) << "|" << (revVal.empty() ? "(not found)" : revVal) << "|" << (pageNum + 1) << std::endl;
        }
    }
    csv.close();
    std::cout << "CSV output written to dwg_rev_output.csv" << std::endl;
    return 0;
}


// Utility: trim whitespace
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string getRevNumberPattern(const std::string& revText) {
    // Match: 0, 1, 2, A, B, C, 0A, 0B, 10, 99, AA, AB, BA, etc. (max 2 chars, digit or letter)
    std::regex re(R"((\b[0-9]{1,2}[A-Z]?\b|\b[A-Z]{1,2}\b))");
    std::smatch m;
    std::string revTextTrimmed = trim(revText);
    // std::cout << "Trimmed revText: '" << revTextTrimmed << "'\n";
    if (std::regex_search(revTextTrimmed, m, re)) {
        return m[1].str();
    }
    return "";
}

// Extract text from a specific region of a PDF page
std::string extractRegionText(const poppler::page& page, double x1, double y1, double x2, double y2) {
    poppler::rectf rect(x1, y1, x2, y2);
    poppler::ustring utext = page.text(rect, poppler::page::physical_layout);
    std::vector<char> utf8 = utext.to_utf8();
    return utf8.empty() ? "" : std::string(utf8.begin(), utf8.end());
}

// Extract the first string after a prefix in a text block
std::string extractAfterPrefix(const std::string& text, const std::string& prefix) {
    std::regex re(prefix + R"([ \t]*:?[ \t]*([\S]+))");
    std::smatch m;
    if (std::regex_search(text, m, re)) {
        return m[1].str();
    }
    return "";
}