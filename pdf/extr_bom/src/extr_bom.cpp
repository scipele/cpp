//| Item         | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | extr_bom.cpp                                                |
//| EntryPoint   | main                                                        |
//| Purpose      | Extract Bill of Materials from piping isometric PDFs        |
//| Inputs       | inp folder containing PDF files with BOM in top right       |
//| Outputs      | CSV file with pipe-delimited BOM data                       |
//| Dependencies | libpoppler-cpp (linked directly, no shell calls)            |
//| By Name,Date | T.Sciple, 01/24/2026                                        |
//|--------------|-------------------------------------------------------------|
//| BOM Format   | ITEM | QTY | SIZE | DESCRIPTION | COMMODITY CODE            |
//| Sections     | SHOP MATERIALS (s) and FIELD MATERIALS (f)                  |
//|--------------|-------------------------------------------------------------|
//| Usage        | extr_bom [inp] [out] [x"] [w"] [y"] [h"]                    |
//| Defaults     | ./inp ./out/bom_output.csv 10.89 5.33 0.81 6.78             |
//| Crop Region  | Extracts only from BOM area (right side of 11x17 drawing)   |
//|--------------|-------------------------------------------------------------|
//| Compile Linux| g++ -std=c++17 ./extr_bom.cpp -o ../bin/extr_bom $(pkg-config --cflags --libs poppler-cpp) |
//| Compile Win  | g++ -std=c++17 extr_bom.cpp -o extr_bom -I<poppler>/include -L<poppler>/lib -lpoppler-cpp |


#include <filesystem>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <regex>
#include <memory>

// Poppler C++ library for direct PDF text extraction (no shell calls)
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-global.h>

namespace fs = std::filesystem;


// BOM line item structure
struct BomItem {
    std::string filename;       // Source PDF filename
    std::string fieldCode;      // 's' for shop, 'f' for field
    std::string item;           // Item number
    std::string qty;            // Quantity
    std::string size;           // Size
    std::string description;    // Description
    std::string commodityCode;  // Commodity code
};


// Global crop settings for BOM region (in points, 72 points = 1 inch)
// Default: BOM area on right side of 11x17 landscape drawing
struct CropRegion {
    int x = 784;    // 10.89" from left
    int y = 58;     // 0.8108" from top
    int w = 384;    // 5.33" wide (to 16.22")
    int h = 488;    // 6.78" tall (to 7.59")
} g_cropRegion;

// Static reject patterns for descriptions (avoid recreating on each call)
static const std::vector<std::string> REJECT_PATTERNS = {
    "COPIED", "REPRODUCED", "BORROWER", "PROPERTY OF", "CONFIDENTIAL",
    "FURNISHED", "DISPOSED", "PUBLISHED", "REFINERIES", "ENGINEERING CO",
    "PROJECT NO", "BMCD", "NOTICE:", "NOTES:"
};


// Function declarations
std::string extractTextFromPdf(const fs::path& pdfPath);
std::vector<BomItem> parseBomText(const std::string& text, const std::string& filename);
std::string trim(const std::string& str);
std::vector<std::string> splitLine(const std::string& line);
std::vector<std::vector<std::string>> extractAllBomFields(const std::string& line);
bool looksLikeDimension(const std::string& field);
bool isValidBomItem(const BomItem& item);
bool isDescriptionContinuation(const std::string& line);
void writeCsv(const std::vector<BomItem>& items, const fs::path& outputPath);


int main(int argc, char* argv[]) {
    // Default paths
    fs::path inputFolder = "../inp";
    fs::path outputFile = "../out/bom_output.csv";

    // Parse command line arguments
    // Usage: extr_bom [input_folder] [output_file] [crop_x"] [crop_w"] [crop_y"] [crop_h"]
    // Process arguments in sequence - switch handles fall-through nicely here
    switch (argc) {
        default:  // argc >= 7
        case 7:
            // Crop height in inches (convert to points)
            g_cropRegion.h = static_cast<int>(std::stod(argv[6]) * 72);
            [[fallthrough]];
        case 6:
            // Crop Y position in inches (convert to points)
            g_cropRegion.y = static_cast<int>(std::stod(argv[5]) * 72);
            [[fallthrough]];
        case 5:
            // Crop width in inches (convert to points)
            g_cropRegion.w = static_cast<int>(std::stod(argv[4]) * 72);
            [[fallthrough]];
        case 4:
            // Crop X position in inches (convert to points)
            g_cropRegion.x = static_cast<int>(std::stod(argv[3]) * 72);
            [[fallthrough]];
        case 3:
            outputFile = fs::u8path(argv[2]);
            [[fallthrough]];
        case 2:
            inputFolder = fs::u8path(argv[1]);
            [[fallthrough]];
        case 1:
        case 0:
            break;
    }

    std::cout << "BOM Extractor - Piping Isometrics" << std::endl;
    std::cout << "Input folder: " << inputFolder.u8string() << std::endl;
    std::cout << "Output file:  " << outputFile.u8string() << std::endl;
    std::cout << "Crop region:  x=" << (g_cropRegion.x / 72.0) << "\" y=" << (g_cropRegion.y / 72.0) 
              << "\" w=" << (g_cropRegion.w / 72.0) << "\" h=" << (g_cropRegion.h / 72.0) << "\"" << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    // Verify input folder exists
    if (!fs::exists(inputFolder) || !fs::is_directory(inputFolder)) {
        std::cerr << "Error: Input folder does not exist: " << inputFolder.u8string() << std::endl;
        return 1;
    }

    std::vector<BomItem> allItems;
    int successCount = 0;
    int failCount = 0;

    // Count and collect all PDF files
    std::vector<fs::path> pdfFiles;
    int pdfCount = 0;
    for (const auto& entry : fs::directory_iterator(inputFolder)) {
        if (entry.is_regular_file() && entry.path().extension() == ".pdf") {
            pdfCount++;
        }
    }
    
    // Reserve space for PDFs and estimate for items (~10 items per PDF)
    pdfFiles.reserve(pdfCount);
    allItems.reserve(pdfCount * 10);
    
    for (const auto& entry : fs::directory_iterator(inputFolder)) {
        if (entry.is_regular_file() && entry.path().extension() == ".pdf") {
            pdfFiles.push_back(entry.path());
        }
    }
    std::sort(pdfFiles.begin(), pdfFiles.end());

    // Process each PDF file in sorted order
    for (const auto& pdfPath : pdfFiles) {
        std::string filename = pdfPath.stem().string();

        std::cout << "Processing: " << filename << std::endl;

        // Extract text from PDF
        std::string text = extractTextFromPdf(pdfPath);
        
        if (text.empty()) {
            std::cerr << "  Warning: No text extracted from " << filename << std::endl;
            failCount++;
            continue;
        }

        // Parse BOM from extracted text
        std::vector<BomItem> items = parseBomText(text, filename);
        
        if (items.empty()) {
            std::cerr << "  Warning: No BOM items found in " << filename << std::endl;
            failCount++;
        } else {
            std::cout << "  Found " << items.size() << " BOM items" << std::endl;
            allItems.insert(allItems.end(), items.begin(), items.end());
            successCount++;
        }
    }

    // Write CSV output
    if (!allItems.empty()) {
        writeCsv(allItems, outputFile);
        std::cout << std::string(50, '-') << std::endl;
        std::cout << "Output written to: " << outputFile.u8string() << std::endl;
    }

    std::cout << "\nComplete: " << successCount << " succeeded, " << failCount << " failed." << std::endl;
    std::cout << "Total BOM items extracted: " << allItems.size() << std::endl;
    
    return (failCount > 0 && successCount == 0) ? 1 : 0;
}


// Trim whitespace from both ends of a string
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}


// Extract text from PDF using Poppler C++ library directly (no shell calls)
// Extracts only from the BOM region on the right side of the drawing
std::string extractTextFromPdf(const fs::path& pdfPath) {
    // Load the PDF document
    std::unique_ptr<poppler::document> doc(
        poppler::document::load_from_file(pdfPath.string())
    );
    
    if (!doc) {
        std::cerr << "Error: Could not load PDF: " << pdfPath.u8string() << std::endl;
        return "";
    }
    
    if (doc->is_locked()) {
        std::cerr << "Error: PDF is password protected: " << pdfPath.u8string() << std::endl;
        return "";
    }
    
    std::string allText;
    
    // Process each page
    for (int i = 0; i < doc->pages(); ++i) {
        std::unique_ptr<poppler::page> page(doc->create_page(i));
        if (!page) continue;
        
        // Get page dimensions to calculate crop rectangle
        poppler::rectf pageRect = page->page_rect();
        
        // Define crop region in PDF coordinates
        // Note: PDF coordinates have origin at bottom-left, but poppler uses top-left
        // Crop region: x, y (from top), width, height (all in points)
        poppler::rectf cropRect(
            static_cast<double>(g_cropRegion.x),
            static_cast<double>(g_cropRegion.y),
            static_cast<double>(g_cropRegion.x + g_cropRegion.w),
            static_cast<double>(g_cropRegion.y + g_cropRegion.h)
        );
        
        // Extract text from the crop region with physical layout
        // Use raw_order_layout for layout-preserving extraction
        poppler::ustring pageText = page->text(cropRect, poppler::page::physical_layout);
        
        // Convert poppler::ustring (UTF-16/32) to std::string (UTF-8)
        std::vector<char> utf8 = pageText.to_utf8();
        if (!utf8.empty()) {
            allText += std::string(utf8.begin(), utf8.end());
            allText += "\n";
        }
    }
    
    return allText;
}


// Split a BOM line into fields based on whitespace patterns
// This handles the columnar layout from pdftotext -layout
std::vector<std::string> splitLine(const std::string& line) {
    std::vector<std::string> fields;
    
    // Use regex to split on 2+ spaces (column separators in layout mode)
    std::regex multiSpace("\\s{2,}");
    std::sregex_token_iterator iter(line.begin(), line.end(), multiSpace, -1);
    std::sregex_token_iterator end;
    
    for (; iter != end; ++iter) {
        std::string field = trim(*iter);
        if (!field.empty()) {
            fields.push_back(field);
        }
    }
    
    return fields;
}


// Try to extract BOM data from a line - returns empty vector if not a valid BOM line
// BOM pattern: ITEM(1-2 digits) QTY SIZE DESCRIPTION COMMODITY_CODE
// The BOM data may appear anywhere on the line due to PDF layout
// Returns ALL valid BOM items found on the line (there may be multiple due to PDF layout)
std::vector<std::vector<std::string>> extractAllBomFields(const std::string& line) {
    std::vector<std::vector<std::string>> results;
    
    // Pattern for BOM data - description can start with digit (e.g., "90 Deg.") or letter
    // Description is captured more carefully - stop before commodity code pattern
    // Commodity code is typically: letters/numbers/hyphens, 5+ chars, often starts with letter
    std::regex bomPattern(R"((\d{1,2})\s{2,}(\d+\.?\d*'?)\s{2,}(\d+(?:X\d+)?(?:/\d+)?)\s{2,}((?:\d+\s*Deg\.|[A-Za-z])[^|]*?)\s{2,}([A-Z][A-Z0-9-]{4,}|PS-[A-Z0-9-]+|AS-[A-Z0-9-]+|VGA\d+|\d{7}))");
    
    // Find all matches in the line
    std::string::const_iterator searchStart = line.cbegin();
    std::smatch match;
    
    while (std::regex_search(searchStart, line.cend(), match, bomPattern)) {
        std::string itemNum = match[1].str();
        std::string qty = match[2].str();
        std::string size = match[3].str();
        std::string desc = trim(match[4].str());
        std::string commCode = trim(match[5].str());
        
        // Validate item number is reasonable (1-99)
        int item = 0;
        try {
            item = std::stoi(itemNum);
            if (item < 1 || item > 99) {
                searchStart = match.suffix().first;
                continue;
            }
        } catch (...) {
            searchStart = match.suffix().first;
            continue;
        }
        
        // Reject if qty looks like temperature/pressure
        if (qty.find("F") != std::string::npos || qty.find("psig") != std::string::npos) {
            searchStart = match.suffix().first;
            continue;
        }
        
        // Clean up description - remove trailing junk
        // Stop at patterns that indicate end of description
        std::regex descEndPattern(R"(\s{3,}|\bCOPIED\b|\bNOTES\b|\bNOTICE\b|\b[NWSE]\s+\d+'-|\bEL\s+[+-])");
        std::smatch descEnd;
        if (std::regex_search(desc, descEnd, descEndPattern)) {
            desc = trim(desc.substr(0, descEnd.position()));
        }
        
        // Reject descriptions that contain disqualifying patterns
        std::string upperDesc = desc;
        std::transform(upperDesc.begin(), upperDesc.end(), upperDesc.begin(), ::toupper);
        
        bool hasRejectPattern = false;
        for (const auto& pattern : REJECT_PATTERNS) {
            if (upperDesc.find(pattern) != std::string::npos) {
                hasRejectPattern = true;
                break;
            }
        }
        
        if (!hasRejectPattern && desc.length() >= 3) {
            std::vector<std::string> candidate = {itemNum, qty, size, desc, commCode};
            results.push_back(candidate);
        }
        
        searchStart = match.suffix().first;
    }
    
    return results;
}


// Check if a line looks like a BOM data line (has item number pattern)
bool isBomDataLine(const std::string& line) {
    std::string trimmed = trim(line);
    if (trimmed.empty()) return false;
    
    // BOM lines must have a simple integer item number (1, 2, 3, etc.)
    // followed by 2+ spaces (column separator), then a quantity
    // Pattern can appear anywhere in the line due to PDF layout
    std::regex itemPattern(R"(\b(\d{1,2})\s{2,}\d+\.?\d*'?\s{2,})");
    std::smatch match;
    
    if (!std::regex_search(line, match, itemPattern)) {
        return false;
    }
    
    // Check what comes after - reject temperature patterns
    std::string afterItem = match.suffix().str();
    std::string trimmedAfter = trim(afterItem);
    
    // Reject if the "quantity" field looks like a temperature (e.g., "150 F", "270 psig")
    std::regex tempPattern("^\\d+\\s*[Ff]\\s");
    if (std::regex_search(trimmedAfter, tempPattern)) {
        return false;
    }
    
    return true;
}


// Check if the first field looks like a dimension rather than an item number
bool looksLikeDimension(const std::string& field) {
    // Dimensions typically have fractions, inch marks, or feet marks
    // Examples: "2 1/2"", "5 9/16"", "19 3/4"", "7'-0"
    std::regex dimPattern(R"((\d+\s+)?\d+\s*/\s*\d+|\d+['"]|^\d+\s+\d+/)");
    return std::regex_search(field, dimPattern);
}


// Check if a BOM item looks valid based on structure, not keywords
bool isValidBomItem(const BomItem& item) {
    // Item number should be a reasonable integer (1-99 typically)
    try {
        int itemNum = std::stoi(item.item);
        if (itemNum < 1 || itemNum > 99) return false;
    } catch (...) {
        return false;
    }
    
    // Quantity should look like a number or measurement (e.g., "1", "3", "79.8'")
    std::regex qtyPattern(R"(^\d+\.?\d*'?$)");
    if (!std::regex_match(item.qty, qtyPattern)) {
        return false;
    }
    
    // Description should have reasonable length
    if (item.description.length() < 3) {
        return false;
    }
    
    // Reject descriptions that contain disqualifying patterns (legal text, metadata)
    std::string upperDesc = item.description;
    std::transform(upperDesc.begin(), upperDesc.end(), upperDesc.begin(), ::toupper);
    
    for (const auto& pattern : REJECT_PATTERNS) {
        if (upperDesc.find(pattern) != std::string::npos) {
            return false;
        }
    }
    
    return true;
}


// Check if a line looks like a description continuation (no item number, but has text)
bool isDescriptionContinuation(const std::string& line) {
    std::string trimmed = trim(line);
    if (trimmed.empty()) return false;
    
    // Continuation lines should NOT start with a digit (item number)
    // and should NOT look like a section header
    if (std::isdigit(trimmed[0])) return false;
    
    std::string upperLine = trimmed;
    std::transform(upperLine.begin(), upperLine.end(), upperLine.begin(), ::toupper);
    
    // Skip section headers and column headers
    if (upperLine.find("SHOP MATERIAL") != std::string::npos ||
        upperLine.find("FIELD MATERIAL") != std::string::npos ||
        (upperLine.find("ITEM") != std::string::npos && upperLine.find("QTY") != std::string::npos)) {
        return false;
    }
    
    // Skip lines that look like drawing metadata (revision info, dates, etc.)
    if (upperLine.find("REV") == 0 || upperLine.find("DATE") != std::string::npos ||
        upperLine.find("DRAWN") != std::string::npos || upperLine.find("CHECKED") != std::string::npos ||
        upperLine.find("APPROVED") != std::string::npos || upperLine.find("SCALE") != std::string::npos) {
        return false;
    }
    
    // Skip legal disclaimer and stamp text
    if (upperLine.find("COPIED") != std::string::npos ||
        upperLine.find("INDIRECTLY,") != std::string::npos ||        
        upperLine.find("REPRODUCED") != std::string::npos ||
        upperLine.find("BORROWER") != std::string::npos ||
        upperLine.find("PROPERTY OF") != std::string::npos ||
        upperLine.find("CONFIDENTIAL") != std::string::npos ||
        upperLine.find("FURNISHED") != std::string::npos ||
        upperLine.find("DISPOSED") != std::string::npos ||
        upperLine.find("NOTES:") != std::string::npos ||
        upperLine.find("NOTICE:") != std::string::npos ||
        upperLine.find("DRAWING") != std::string::npos ||
        upperLine.find("PUBLISHED") != std::string::npos ||
        upperLine.find("REFINERIES") != std::string::npos ||
        upperLine.find("ENGINEERING CO") != std::string::npos ||
        upperLine.find("PROJECT NO") != std::string::npos ||
        upperLine.find("DIMENSIONS") != std::string::npos ||
        upperLine.find("MICHAEL") != std::string::npos ||
        upperLine.find("MERRITT") != std::string::npos ||
        upperLine.find("STRESS RELIEVED") != std::string::npos ||
        upperLine.find("RADIOGRAPH") != std::string::npos ||
        upperLine.find("HYDROTEST") != std::string::npos ||
        upperLine.find("TRACER") != std::string::npos ||
        upperLine.find("INSULATION") != std::string::npos ||
        upperLine.find("TEMPERATURE") != std::string::npos ||
        upperLine.find("PRESSURE") != std::string::npos ||
        upperLine.find("VAPOR/LIQUID") != std::string::npos ||
        upperLine.find("PIPING PLAN") != std::string::npos ||
        upperLine.find("LIQUID") == 0 ||
        upperLine.find("CODE") == 0 ||
        upperLine.find("PAINT") != std::string::npos ||
        upperLine.find("SPEC.") != std::string::npos ||
        upperLine.find("DESIGN") != std::string::npos ||
        upperLine.find("FLOW") != std::string::npos ||
        upperLine.find("PSIG") != std::string::npos ||
        upperLine.find("N/A") != std::string::npos ||
        upperLine.find("PURPOSE OTHER THAN") != std::string::npos ||
        upperLine.find("NY PURPOSE") != std::string::npos ||
        upperLine.find("OTHER THAN") != std::string::npos) {
        return false;
    }
    
    // Skip coordinate/location text (patterns like "W 850'-8", "N 3319'-8", "EL +30'")
    std::regex coordPattern(R"([NWSE]\s*\d+['-]|EL\s*[+-]?\d+)");
    if (std::regex_search(upperLine, coordPattern)) {
        return false;
    }
    
    // Skip lines with weld numbers like W170636DB, W2E1731CC (start with 'W')
    // Avoid filtering material grades like ASTM A106, A234, etc.
    std::regex weldNumPattern(R"(\bW[A-Z0-9]{4,}\b)");
    if (std::regex_search(upperLine, weldNumPattern)) {
        return false;
    }
    
    // Skip lines that look like support tags (PS-xxx, AS-xxx patterns with additional text)
    std::regex supportTagPattern(R"(\b[PA]S-\d{3,4}\b)");
    std::smatch tagMatch;
    if (std::regex_search(upperLine, tagMatch, supportTagPattern)) {
        // If the line has a support tag but also has lots of other text, skip it
        if (trimmed.length() > 30) {
            return false;
        }
    }
    
    // Skip lines with dates (month names or date patterns)
    std::regex datePattern(R"(January|February|March|April|May|June|July|August|September|October|November|December|\d{1,2}/\d{1,2}/\d{2,4})");
    if (std::regex_search(line, datePattern)) {
        return false;
    }
    
    // Continuation should be reasonably short
    if (trimmed.length() > 100) return false;
    
    // Must have at least some content (letters)
    bool hasLetters = false;
    for (char c : trimmed) {
        if (std::isalpha(c)) {
            hasLetters = true;
            break;
        }
    }
    
    return hasLetters;
}


// Parse BOM text and extract items
std::vector<BomItem> parseBomText(const std::string& text, const std::string& filename) {
    std::vector<BomItem> items;
    std::istringstream stream(text);
    std::string line;
    
    std::string currentSection = "";  // Track current section (shop/field)
    bool inBomSection = false;
    
    while (std::getline(stream, line)) {
        std::string trimmedLine = trim(line);

        // std::cout << "DEBUG: Processing line: " << trimmedLine << std::endl;

        std::string upperLine = trimmedLine;
        std::transform(upperLine.begin(), upperLine.end(), upperLine.begin(), ::toupper);
        
        // Check for section headers
        if (upperLine.find("SHOP MATERIAL") != std::string::npos ||
            upperLine.find("SHOP MATERIALS") != std::string::npos) {
            currentSection = "s";
            inBomSection = true;
            continue;
        }
        
        if (upperLine.find("FIELD MATERIAL") != std::string::npos ||
            upperLine.find("FIELD MATERIALS") != std::string::npos) {
            currentSection = "f";
            inBomSection = true;
            continue;
        }
        
        // Check for BOM column headers (skip these)
        if (upperLine.find("ITEM") != std::string::npos && 
            upperLine.find("QTY") != std::string::npos) {
            inBomSection = true;
            continue;
        }
        
        // Skip if we haven't found a section yet
        if (currentSection.empty()) {
            continue;
        }
        
        // Skip empty lines
        if (trimmedLine.empty()) {
            continue;
        }
        
        // Try to extract ALL BOM fields using regex pattern matching
        // This handles lines where multiple BOM items appear due to PDF layout
        std::vector<std::vector<std::string>> allFields = extractAllBomFields(line);
        
        // Process all found BOM items on this line
        if (!allFields.empty()) {
            for (const auto& fields : allFields) {
                if (fields.size() == 5) {
                    BomItem item;
                    item.filename = filename;
                    item.fieldCode = currentSection;
                    item.item = fields[0];
                    item.qty = fields[1];
                    item.size = fields[2];
                    item.description = fields[3];
                    item.commodityCode = fields[4];
                    
                    // Only add if it passes validation
                    if (isValidBomItem(item)) {
                        items.push_back(item);
                    }
                }
            }
            continue;
        }
        
        // Fallback: try the old split-based parsing for simpler lines
        if (isBomDataLine(line)) {
            std::vector<std::string> fields = splitLine(line);
            
            // We expect at least ITEM, QTY, SIZE, DESCRIPTION
            if (fields.size() >= 4) {
                BomItem item;
                item.filename = filename;
                item.fieldCode = currentSection;
                item.item = fields[0];
                item.qty = fields[1];
                item.size = fields[2];
                item.description = fields[3];
                
                // Commodity code is optional (5th field)
                if (fields.size() >= 5) {
                    item.commodityCode = fields[4];
                    for (size_t i = 5; i < fields.size(); ++i) {
                        item.commodityCode += " " + fields[i];
                    }
                }
                
                // Only add if it passes validation
                if (isValidBomItem(item)) {
                    items.push_back(item);
                }
            }
            continue;
        }
        
        // Append continuation lines (wrapped descriptions) to the previous item
        if (!items.empty() && inBomSection && isDescriptionContinuation(line)) {
            std::string continuation = trim(line);
            items.back().description += " " + continuation;
        }
    }
    
    return items;
}


// Write BOM items to CSV file with pipe delimiter
void writeCsv(const std::vector<BomItem>& items, const fs::path& outputPath) {
    std::ofstream file(outputPath);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not create output file: " << outputPath.u8string() << std::endl;
        return;
    }
    
    // Write delimiter hint for Excel/LibreOffice
    file << "sep=|" << std::endl;
    
    // Write header
    file << "FILENAME|FIELD_CODE|ITEM|QTY|SIZE|DESCRIPTION|COMMODITY_CODE" << std::endl;
    
    // Write data rows
    for (const auto& item : items) {
        file << item.filename << "|"
             << item.fieldCode << "|"
             << item.item << "|"
             << item.qty << "|"
             << item.size << "|"
             << item.description << "|"
             << item.commodityCode << std::endl;
    }
    
    file.close();
}