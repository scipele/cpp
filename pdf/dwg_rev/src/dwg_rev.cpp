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
//| Compile Linux| g++ -std=c++17 ./dwg_rev.cpp -o ../bin/dwg_rev $(pkg-config --cflags --libs poppler-cpp) g++ -std=c++17 -o ../bin/dwg_rev.exe dwg_rev.cpp $(pkg-config --cflags --libs poppler-cpp) -lstdc++
//| Compile in MSYS2 UCRT64 Term-> g++ -std=c++17 -mconsole -o ../bin/dwg_rev.exe dwg_rev.cpp $(pkg-config --cflags --libs poppler-cpp) -lstdc++


#include <filesystem>
#include <string>
#include <iostream>
#include <regex>
#include <memory>
#include <fstream>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-global.h>
#ifdef _WIN32
#include <windows.h>
#endif

// box coordinates measured in inches.  Multiply by 72 to convert to points.
struct box_coord {
    double x;
    double y;
    double w;
    double h;
};


// relative offsets for text box extraction
struct relative_offset {
    double heightRelToLabel;
    double yPosRelToLabelHeight;
    double widthRelToLabel;
    double xPosRelToLabelWidth;
};


namespace fs = std::filesystem;

// Function prototypes
box_coord getTextBoxCoords(poppler::page& page, const relative_offset& offset, std::string searchString);
fs::path getExecutableDir();
std::string trim(const std::string& str);
std::string getRevNumberPattern(const std::string& revText);
std::string extractRegionText(const poppler::page& page, box_coord);
std::string extractAfterPrefix(const std::string& text, const std::string& prefix);
std::string cleanDocNo(const std::string& docNoText);
void listTextBoxes(const poppler::page& page);

void pauseConsole();


int main(int argc, char* argv[]) {

// Print debug info for text box coordinates for certain strings

    // In Poppler, the coordinates (x, y) for a region or bounding box are measured in points (1 inch = 72 points),
    // and they are relative to the bottom-left corner of the PDF page. So:
    // 
    // The origin (0, 0) is at the bottom-left of the sheet unless printed rotated.
    // The coordinates you specify (e.g., x=15.0, y=10.70) represent the bottom-left corner of the region you want to extract.
    // The width and height then extend rightward and upward from that point.
    // Your region:
    // 
    // x, y = bottom-left of the text box
    // w, h = width and height in inches (converted to points)

    // struct relative_offset {
    //     double heightRelToLabel;
    //     double yPosRelToLabelHeight;
    //     double widthRelToLabel;
    //     double xPosRelToLabelWidth;

    std::string adjDocNoLabel = "DRAWING NUMBER";
    const relative_offset offsetDocNo = { 2.0, 1.0, 2.5, 0.0 }; // unchanged

    std::string adjRevLabel = "REVISION";
    // Adjusted for vertical layout: height, y offset, width, x offset (all in inches)
    const relative_offset offsetRev = { 0.3, 0.7, 0.5, 0.25 }; // 0.3in tall, 0.7in down, 0.5in wide, 0.25in right of label

    // Ask user if they want to process only page 1
    bool page1only = false;
    std::string userInput;
    std::cout << "Process only page 1 of each PDF? (y/n): ";
    std::getline(std::cin, userInput);
    if (!userInput.empty() && (userInput[0] == 'y' || userInput[0] == 'Y')) {
        page1only = true;
    }

    // Get executable directory (cross-platform)
    fs::path exeDir = getExecutableDir();
    // std::cout << "Executable directory: " << exeDir.u8string() << std::endl;
    
    fs::path inputFolder = exeDir / "inp";
    // std::cout << "Input folder: " << inputFolder.u8string() << std::endl;

    if (!fs::exists(inputFolder) || !fs::is_directory(inputFolder)) {
        std::cerr << "Error: Input folder does not exist: " << inputFolder.u8string() << std::endl;
        return 1;
    }

    fs::path outputFile = exeDir / "out" / "dwg_rev_output.csv";
    std::cout << "Output file: " << outputFile.u8string() << std::endl;

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
        int startPage = 0;
        int endPage = page1only ? 1 : numPages;
        for (int pageNum = startPage; pageNum < endPage; ++pageNum) {
            std::unique_ptr<poppler::page> page(doc->create_page(pageNum));
            if (!page) continue;
            // Debug only ->  listTextBoxes(*page);
            box_coord bxDoc = getTextBoxCoords(*page, offsetDocNo, adjDocNoLabel); 
            std::cout << "filename: " << fname << ", page: " << (pageNum + 1) << std::endl;
            std::cout << "DocNo box coords: x=" << bxDoc.x << ", y=" << bxDoc.y << ", w=" << bxDoc.w << ", h=" << bxDoc.h << std::endl;
            std::string docNoText = extractRegionText(*page, bxDoc);
            std::string docNoClean = cleanDocNo(docNoText);

            box_coord bxRev = getTextBoxCoords(*page, offsetRev, adjRevLabel); 
            std::cout << "filename: " << fname << ", page: " << (pageNum + 1) << std::endl;
            std::cout << "Rev box coords: x=" << bxRev.x << ", y=" << bxRev.y << ", w=" << bxRev.w << ", h=" << bxRev.h << std::endl;
            std::string revText = extractRegionText(*page, bxRev);
            std::string revVal = getRevNumberPattern(revText);
            csv << fname << "|" << (docNoClean.empty() ? "(not found)" : docNoClean) << "|" << (revVal.empty() ? "(not found)" : revVal) << "|" << (pageNum + 1) << std::endl;
        }
    }
    csv.close();
    std::cout << "CSV output written to dwg_rev_output.csv" << std::endl;
    pauseConsole();
    return 0;
}



// Get box coordinates for specific strings
box_coord getTextBoxCoords(poppler::page& page, const relative_offset& offset, std::string searchString) {
    std::vector<poppler::text_box> boxes = page.text_list();
    std::cout << "\n--- DEBUG: Text box coordinates for Label ---\n";
    std::cout << "Searching for label: '" << searchString << "'\n";

    // Support multi-word label search (e.g., "DRAWING NUMBER")
    std::vector<std::string> searchWords;
    size_t pos = 0, found;
    while ((found = searchString.find(' ', pos)) != std::string::npos) {
        searchWords.push_back(searchString.substr(pos, found - pos));
        pos = found + 1;
    }
    searchWords.push_back(searchString.substr(pos));

    // Extract all box texts and convert to uppercase for comparison
    std::vector<std::string> boxTexts;
    for (const auto& box : boxes) {
        auto utf8vec = box.text().to_utf8();
        std::string txt = utf8vec.empty() ? "" : std::string(utf8vec.begin(), utf8vec.end());
        boxTexts.push_back(txt);
    }

    // Search for sequence of words in adjacent boxes
    for (size_t i = 0; i + searchWords.size() - 1 < boxTexts.size(); ++i) {
        bool match = true;
        for (size_t j = 0; j < searchWords.size(); ++j) {
            if (boxTexts[i + j] != searchWords[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            // Optionally, check that y-coordinates are close (same line)
            bool sameLine = true;
            double y0 = boxes[i].bbox().y();
            for (size_t j = 1; j < searchWords.size(); ++j) {
                double yj = boxes[i + j].bbox().y();
                if (std::abs(yj - y0) > 2.0) { // 2 points tolerance
                    sameLine = false;
                    break;
                }
            }
            if (sameLine) {
                // Merge bounding boxes manually
                double min_x = boxes[i].bbox().x();
                double min_y = boxes[i].bbox().y();
                double max_x = boxes[i].bbox().x() + boxes[i].bbox().width();
                double max_y = boxes[i].bbox().y() + boxes[i].bbox().height();
                for (size_t j = 1; j < searchWords.size(); ++j) {
                    auto b = boxes[i + j].bbox();
                    min_x = std::min(min_x, b.x());
                    min_y = std::min(min_y, b.y());
                    max_x = std::max(max_x, b.x() + b.width());
                    max_y = std::max(max_y, b.y() + b.height());
                }
                double rect_x = min_x;
                double rect_y = min_y;
                double rect_w = max_x - min_x;
                double rect_h = max_y - min_y;
                std::cout << "Text: '" << searchString << "' at (x=" << rect_x / 72.0 << ", y=" << rect_y / 72.0 << ", w=" << rect_w / 72.0 << ", h=" << rect_h / 72.0 << ") inches\n";
                std::cout << "About to return box coordinates based on offset...\n";
                std::cout << "rect.x(): " << rect_x << ", rect.y(): " << rect_y << ", rect.width(): " << rect_w << ", rect.height(): " << rect_h << "\n";
                std::cout << "offset.xPosRelToLabelWidth: " << offset.xPosRelToLabelWidth << ", offset.yPosRelToLabelHeight: " << offset.yPosRelToLabelHeight << "\n";
                std::cout << "offset.widthRelToLabel: " << offset.widthRelToLabel << ", offset.heightRelToLabel: " << offset.heightRelToLabel << "\n";
                // Special case: if label is "REVISION", use absolute offset in inches
                if (searchString == "REVISION") {
                    return box_coord{ rect_x / 72.0 + offset.xPosRelToLabelWidth,
                                      rect_y / 72.0 + offset.yPosRelToLabelHeight,
                                      offset.widthRelToLabel,
                                      offset.heightRelToLabel };
                } else {
                    return box_coord{ rect_x / 72.0 + (rect_w / 72.0 * offset.xPosRelToLabelWidth),
                                      rect_y / 72.0 + (rect_h / 72.0 * offset.yPosRelToLabelHeight),
                                      rect_w / 72.0 * offset.widthRelToLabel,
                                      rect_h / 72.0 * offset.heightRelToLabel };
                }
            }
        }
    }

    // Fallback: single-box search (for backward compatibility)
    for (size_t i = 0; i < boxTexts.size(); ++i) {
        if (boxTexts[i].find(searchWords[0]) != std::string::npos) {
            auto rect = boxes[i].bbox();
            std::cout << "Text: '" << boxes[i].text().to_utf8().data() << "' at (x=" << rect.x() / 72.0 << ", y=" << rect.y() / 72.0 << ", w=" << rect.width() / 72.0 << ", h=" << rect.height() / 72.0 << ") inches\n";
            std::cout << "About to return box coordinates based on offset...\n";
            std::cout << "rect.x(): " << rect.x() << ", rect.y(): " << rect.y() << ", rect.width(): " << rect.width() << ", rect.height(): " << rect.height() << "\n";
            std::cout << "offset.xPosRelToLabelWidth: " << offset.xPosRelToLabelWidth << ", offset.yPosRelToLabelHeight: " << offset.yPosRelToLabelHeight << "\n";
            std::cout << "offset.widthRelToLabel: " << offset.widthRelToLabel << ", offset.heightRelToLabel: " << offset.heightRelToLabel << "\n";
            return box_coord{ rect.x() / 72.0 + (rect.width() / 72.0 * offset.xPosRelToLabelWidth),
                              rect.y() / 72.0 + (rect.height() / 72.0 * offset.yPosRelToLabelHeight),
                              rect.width() / 72.0 * offset.widthRelToLabel,
                              rect.height() / 72.0 * offset.heightRelToLabel};
        }
    }
    // If not found, return zeros
    std::cout << "Did not find the text box for label: '" << searchString << "'\n";
    return box_coord{ 0, 0, 0, 0 };
}


// Cross-platform: get directory of running executable
fs::path getExecutableDir() {
#ifdef _WIN32
    char exePath[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, exePath, MAX_PATH);
    if (len > 0) {
        return fs::path(exePath).parent_path().parent_path();
    } else {
        return fs::current_path();
    }
#elif defined(__linux__)
    char exePath[4096];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len > 0) {
        exePath[len] = '\0';
        return fs::path(exePath).parent_path();
    } else {
        return fs::current_path();
    }
#else
    // Fallback: use current working directory
    return fs::current_path();
#endif
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
    std::cout << "Trimmed revText: '" << revTextTrimmed << "'\n";
    if (std::regex_search(revTextTrimmed, m, re)) {
        return m[1].str();
    }
    return "";
}


// Extract text from a specific region of a PDF page
std::string extractRegionText(const poppler::page& page, box_coord box) {
    poppler::rectf rect(box.x * 72, box.y * 72, (box.x + box.w) * 72, (box.y + box.h) * 72);
    poppler::ustring utext = page.text(rect, poppler::page::physical_layout);
    std::vector<char> utf8 = utext.to_utf8();
    return utf8.empty() ? "" : std::string(utf8.begin(), utf8.end());
}


std::string cleanDocNo(const std::string& docNoText) {
    // Remove any unwanted characters and trim whitespace
    std::string cleaned = trim(docNoText);
    // Remove any non-alphanumeric characters except hyphens
    cleaned = std::regex_replace(cleaned, std::regex(R"([^A-Za-z0-9\-])"), "");
    return cleaned;
}


void pauseConsole() {
    #ifdef _WIN32
        system("pause");
    #else
        std::cout << "Press Enter to continue...";
        std::cin.get();
    #endif
}


void listTextBoxes(const poppler::page& page) {
    std::vector<poppler::text_box> boxes = page.text_list();
    std::cout << "\n--- DEBUG: Listing all text boxes on page ---\n";   
    int idx = 0;
    for (const auto& box : boxes) {
        auto utf8vec = box.text().to_utf8();
        std::string txt = utf8vec.empty() ? "" : std::string(utf8vec.begin(), utf8vec.end());
        auto rect = box.bbox();
        std::cout << "Box " << idx++ << ": '" << txt << "' at (x=" << rect.x() / 72.0
                << ", y=" << rect.y() / 72.0
                << ", w=" << rect.width() / 72.0
                << ", h=" << rect.height() / 72.0 << ") inches\n";
    }
}