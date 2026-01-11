#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <curl/curl.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Function Prototypes
static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
std::string strip_tags(const std::string& input, bool keep_verse_num = false, bool keep_footnotes = true);
std::string fetch_verse(const std::string& ref);
std::string get_executable_directory_parent();
static void remove_crossrefs(std::string& text);

int main() {
    std::string current_path = get_executable_directory_parent();
    std::cout << "Current Path: " << current_path << "\n";

    std::string input_path = current_path + "/input/lookup_verses.txt";
    std::cout << "Current Path: " << input_path << "\n";
    
    std::vector<std::string> refs;
    std::ifstream file(input_path);
    if (!file) {
        std::cerr << "Failed to open input file at: " << input_path << std::endl;
        return 1;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) refs.push_back(line);
    }
    file.close();

    std::string output_path = current_path + "/output/lookup_verses_output.txt";
    std::cout << "Output Path: " << output_path << "\n";
    
    std::ofstream output_file(output_path);
    if (!output_file) {
        std::cerr << "Failed to create output file at: " << output_path << std::endl;
        return 1;
    }

    std::cout << "Bible Verses Looked Up:\n\n";
    output_file << "Bible Verses Looked Up:\n\n";

    for (size_t i = 0; i < refs.size(); ++i) {
        const auto& ref = refs[i];
        std::string verse = fetch_verse(ref);
        if (!verse.empty()) {
            std::cout << ref << "\n" << verse << "\n";
            output_file << ref << "\n" << verse << "\n";
        } else {
            std::cout << ref << ": Failed to fetch verse.\n";
            output_file << ref << "\nFailed to fetch verse\n";
        }
        if (i < refs.size() - 1) {
            std::cout << "\n";
            output_file << "\n";
        }
    }

    output_file.close();
    std::cout << "\nResults saved to " << output_path << "\n";

    return 0;
}

// =====================================================
// Curl Write Callback
// =====================================================
static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    ((std::string*)userdata)->append(ptr, size * nmemb);
    return size * nmemb;
}

// =====================================================
// Strip HTML Tags
// =====================================================
std::string strip_tags(const std::string& input, bool keep_verse_num, bool keep_footnotes) {
    std::string output;
    bool in_tag = false;
    bool in_verse_num = false;
    bool in_crossreference = false;
    bool in_footnote = false;
    for (size_t i = 0; i < input.length(); ++i) {
        if (input[i] == '<') {
            in_tag = true;
            if (keep_verse_num) {
                if (i + 16 < input.length() && input.substr(i, 16) == "<sup class=\"versenum\">") {
                    in_verse_num = true;
                    in_tag = false;
                } else if (i + 17 < input.length() && input.substr(i, 17) == "<span class=\"chapternum\">") {
                    in_verse_num = true;
                    in_tag = false;
                }
            }
            if (i + 17 < input.length() && input.substr(i, 17) == "<sup class=\"crossreference\"") {
                in_crossreference = true;
                in_tag = false;
            }
            if (!keep_footnotes && i + 5 < input.length() && input.substr(i, 5) == "<sup data") {
                in_footnote = true;
                in_tag = false;
            }
        } else if (input[i] == '>') {
            in_tag = false;
            if (in_verse_num) in_verse_num = false;
            if (in_crossreference) in_crossreference = false;
            if (in_footnote) in_footnote = false;
        } else if (!in_tag && !in_crossreference && !in_footnote) {
            output += input[i];
        }
    }

    // Clean up HTML entities
    std::string result;
    for (size_t i = 0; i < output.length(); ++i) {
        if (output[i] == '&') {
            if (i + 6 <= output.length() && output.substr(i, 6) == "&quot;") {
                result += "\""; i += 5;
            } else if (i + 5 <= output.length() && output.substr(i, 5) == "&amp;") {
                result += "&"; i += 4;
            } else if (i + 6 <= output.length() && output.substr(i, 6) == "&nbsp;") {
                i += 5; // skip
            } else {
                result += output[i];
            }
        } else {
            result += output[i];
        }
    }

    // Trim whitespace
    size_t start = result.find_first_not_of(" \n\r\t\xC2\xA0");
    size_t end = result.find_last_not_of(" \n\r\t\xC2\xA0");
    if (start == std::string::npos || end == std::string::npos) return "";

    std::string trimmed = result.substr(start, end - start + 1);
    return trimmed;
}

// =====================================================
// Fetch and Process Verse HTML
// =====================================================
std::string fetch_verse(const std::string& ref) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Error: Failed to initialize curl for " << ref << ".\n";
        return "";
    }

    char *encoded = curl_easy_escape(curl, ref.c_str(), 0);
    std::string url = "https://www.biblegateway.com/passage/?search=" + std::string(encoded) + "&version=NIV";
    curl_free(encoded);

    std::string html;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
#ifdef _WIN32
    Sleep(1000); // Prevent rate limiting
#else
    sleep(1); // Prevent rate limiting
#endif
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK || html.size() < 1000) {
        std::cerr << "Error: Failed to fetch or short HTML for " << ref << "\n";
        return "";
    }

    // --- Find start of main text ---
    size_t chap_pos = html.find("<span class=\"chapternum\">");
    size_t verse_pos = html.find("<sup class=\"versenum\">");
    size_t std_text_start = std::string::npos;

    if (chap_pos != std::string::npos && verse_pos != std::string::npos)
        std_text_start = (chap_pos < verse_pos) ? chap_pos : verse_pos;
    else if (chap_pos != std::string::npos)
        std_text_start = chap_pos;
    else if (verse_pos != std::string::npos)
        std_text_start = verse_pos;

    // Move past closing tag
    if (std_text_start != std::string::npos) {
        if (std_text_start == chap_pos) {
            std_text_start = html.find("</span>", chap_pos);
            if (std_text_start != std::string::npos) std_text_start += 7;
        } else {
            std_text_start = html.find("</sup>", verse_pos);
            if (std_text_start != std::string::npos) std_text_start += 6;
        }
    }

    if (std_text_start == std::string::npos) {
        std::cerr << "Error: Could not find text start for " << ref << "\n";
        return "";
    }

    // --- Find end and isolate text block ---
    size_t std_text_end = html.find("</div>", std_text_start);
    if (std_text_end == std::string::npos) {
        std::cerr << "Error: No closing </div> found for " << ref << "\n";
        return "";
    }

    std::string std_text = html.substr(std_text_start, std_text_end - std_text_start);

    // Extract clean text
    std::string verse_text = strip_tags(std_text, false, true);
    
    // Remove cross-references within the text
    remove_crossrefs(verse_text);

    // Trim
    size_t start = verse_text.find_first_not_of(" \n\r\t");
    size_t end = verse_text.find_last_not_of(" \n\r\t");
    if (start != std::string::npos && end != std::string::npos)
        verse_text = verse_text.substr(start, end - start + 1);

    return verse_text;
}

// =====================================================
// Get Executable Directory
// =====================================================
std::string get_executable_directory_parent() {
#ifdef _WIN32
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string exe_path = path;
    size_t last_slash = exe_path.find_last_of("\\");
    if (last_slash != std::string::npos) {
        exe_path = exe_path.substr(0, last_slash);
        last_slash = exe_path.find_last_of("\\");
        return (last_slash != std::string::npos) ? exe_path.substr(0, last_slash) : ".";
    }
    return ".";
#else
    return "..";
#endif
}


// -----------------------------------------------------
// Helper: remove cross-reference markers like (A), (B), etc.
// -----------------------------------------------------
static void remove_crossrefs(std::string& text) {
    size_t pos = 0;
    while ((pos = text.find("(", pos)) != std::string::npos) {
        size_t end = text.find(")", pos + 1);
        if (end == std::string::npos) break;

        // Check that the content between ( ) is only letters
        bool only_letters = true;
        for (size_t i = pos + 1; i < end; ++i) {
            if (!isalpha(static_cast<unsigned char>(text[i]))) {
                only_letters = false;
                break;
            }
        }
        if (only_letters && end > pos + 1) {
            text.erase(pos, end - pos + 1);   // erase "(X)"
            continue;                         // start search again at same pos
        }
        pos = end + 1;                        // not a cross-ref, keep looking
    }
}