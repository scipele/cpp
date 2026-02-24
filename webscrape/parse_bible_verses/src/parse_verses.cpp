// ************ MAIN PROGRAM ***************************************************
//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | parse_verses.cpp                                            |
//| EntryPoint   | main                                                        |
//| Purpose      | strip out the Bible verses from a text                      |
//| Inputs       | ../input/raw_text.txt                                       |
//| Outputs      | ../output/lookup_verses.txt                                 |
//| Dependencies | none                                                        |
//| By Name,Date | T.Sciple, 02/22/2026                                        |
// compile with: g++ -std=c++17 -o parse_verses parse_verses.cpp


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <set>

// Function prototypes
// NIV Bible book names (full and common abbreviations)
const std::vector<std::string> BIBLE_BOOKS = {
    // Old Testament
    "Genesis","Exodus","Leviticus","Numbers","Deuteronomy","Joshua","Judges","Ruth",
    "1 Samuel","2 Samuel","1 Kings","2 Kings","1 Chronicles","2 Chronicles","Ezra",
    "Nehemiah","Esther","Job","Psalms","Psalm","Proverbs","Ecclesiastes","Song of Solomon",
    "Isaiah","Jeremiah","Lamentations","Ezekiel","Daniel","Hosea","Joel","Amos",
    "Obadiah","Jonah","Micah","Nahum","Habakkuk","Zephaniah","Haggai","Zechariah","Malachi",
    // New Testament
    "Matthew","Mark","Luke","John","Acts","Romans","1 Corinthians","2 Corinthians",
    "Galatians","Ephesians","Philippians","Colossians","1 Thessalonians","2 Thessalonians",
    "1 Timothy","2 Timothy","Titus","Philemon","Hebrews","James","1 Peter","2 Peter",
    "1 John","2 John","3 John","Jude","Revelation"
    };

std::string escapeRegex(const std::string& str);
std::string buildBooksPattern();
void pauseConsole();


int main() {
    const std::string inputPath = "../input/raw_text.txt";
    const std::string outputPath = "../output/lookup_verses.txt";
    
    // Read input file
    std::ifstream inFile(inputPath);
    if (!inFile.is_open()) {
        std::cerr << "Error: Could not open input file: " << inputPath << std::endl;
        return 1;
    }
    
    std::string content((std::istreambuf_iterator<char>(inFile)),
                         std::istreambuf_iterator<char>());
    inFile.close();
    
    // Build regex pattern for Bible verses
    // Pattern: BookName Chapter:Verse (with optional verse ranges like 1-5 or 1,3,5)
    std::string booksPattern = buildBooksPattern();
    
    // Match patterns like: "John 3:16", "1 Cor 13:1-13", "Genesis 1:1,3,5", "Psalm 23"
    // Use word boundary \b to prevent matching partial words (e.g., "verses" matching "es")
    std::string versePattern = "\\b" + booksPattern + 
        R"(\.?\s+(\d{1,3})(?:\s*:\s*(\d{1,3})(?:\s*[-–—]\s*(\d{1,3}))?(?:\s*,\s*(\d{1,3}(?:\s*[-–—]\s*\d{1,3})?))*)?)";
    
    std::regex verseRegex(versePattern, std::regex::icase);
    
    // Find all matches
    std::vector<std::string> uniqueVerses; // Maintain order of appearance
    std::set<std::string> seenVerses;      // Track duplicates
    std::regex whitespaceRegex(R"(\s+)");  // For normalizing whitespace
    std::sregex_iterator it(content.begin(), content.end(), verseRegex);
    std::sregex_iterator end;
    
    while (it != end) {
        // Normalize whitespace: replace multiple spaces/newlines with single space
        std::string verse = std::regex_replace(it->str(), whitespaceRegex, " ");
        // Only add if not seen before (preserves first occurrence order)
        if (seenVerses.insert(verse).second) {
            uniqueVerses.push_back(verse);
        }
        ++it;
    }
    
    // Write output file (overwrite if exists)
    std::ofstream outFile(outputPath, std::ios::trunc);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open output file: " << outputPath << std::endl;
        return 1;
    }
    
    std::cout << "Found " << uniqueVerses.size() << " unique Bible verse references:" << std::endl;
    
    for (const auto& verse : uniqueVerses) {
        outFile << verse << std::endl;
        std::cout << "  " << verse << std::endl;
    }
    
    outFile.close();
    
    std::cout << "\nVerses written to: " << outputPath << std::endl;
    
    pauseConsole();
    
    return 0;
}


std::string escapeRegex(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c == '.' || c == '^' || c == '$' || c == '|' || c == '(' || 
            c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || 
            c == '*' || c == '+' || c == '?' || c == '\\') {
            result += '\\';
        }
        result += c;
    }
    return result;
}


std::string buildBooksPattern() {
    std::string pattern = "(";
    for (size_t i = 0; i < BIBLE_BOOKS.size(); ++i) {
        if (i > 0) pattern += "|";
        pattern += escapeRegex(BIBLE_BOOKS[i]);
    }
    pattern += ")";
    return pattern;
}


void pauseConsole() {
    // If Windows, use system("pause"); otherwise, use cin.get() to wait for user input
    #ifdef _WIN32
        system("pause");    
    #else
        std::cout << "Press Enter to continue...";
        std::cin.get();
    #endif
}