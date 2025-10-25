#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <curl/curl.h>
#ifdef _WIN32
#include <windows.h>
#endif

// Function Prototypes
static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
std::string strip_tags(const std::string& input);
std::string fetch_verse(const std::string& ref);
std::string get_executable_directory();

int main() {
    // Get the executable's directory and construct the input file path
    std::string input_path = get_executable_directory() + "\\lookup_verses.txt";
    
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

    // Construct the output file path
    std::string output_path = get_executable_directory() + "\\lookup_verses_output.txt";
    
    // Open output file
    std::ofstream output_file(output_path);
    if (!output_file) {
        std::cerr << "Failed to create output file at: " << output_path << std::endl;
        return 1;
    }

    for (size_t i = 0; i < refs.size(); ++i) {
        const auto& ref = refs[i];
        std::string verse = fetch_verse(ref);
        if (!verse.empty()) {
            std::cout << ref << ": " << verse << std::endl;
            output_file << ref << " " << verse;
        } else {
            std::cout << ref << ": Failed to fetch verse." << std::endl;
            output_file << ref << "\nFailed to fetch verse";
        }
        // Add blank line between verses, but not after the last verse
        if (i < refs.size() - 1) {
            output_file << "\n\n";
        }
    }

    output_file.close();
    std::cout << "\nResults saved to " << output_path << std::endl;

    return 0;
}

static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    ((std::string*)userdata)->append(ptr, size * nmemb);
    return size * nmemb;
}

std::string strip_tags(const std::string& input) {
    std::string output;
    bool in_tag = false;
    for (char c : input) {
        if (c == '<') {
            in_tag = true;
        } else if (c == '>') {
            in_tag = false;
        } else if (!in_tag) {
            output += c;
        }
    }
    // Clean up any HTML entities (e.g., &quot;, &amp;)
    std::string result;
    for (size_t i = 0; i < output.length(); ++i) {
        if (output[i] == '&') {
            if (i + 6 <= output.length() && output.substr(i, 6) == "&quot;") {
                result += "\"";
                i += 5;
            } else if (i + 5 <= output.length() && output.substr(i, 5) == "&amp;") {
                result += "&";
                i += 4;
            } else {
                result += output[i];
            }
        } else {
            result += output[i];
        }
    }
    return result;
}

std::string fetch_verse(const std::string& ref) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Error: Failed to initialize curl." << std::endl;
        return "";
    }

    char *encoded = curl_easy_escape(curl, ref.c_str(), 0);
    std::string url = "https://www.biblegateway.com/passage/?search=" + std::string(encoded) + "&version=NIV";
    curl_free(encoded);

    std::string html;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Handle redirects
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Error: curl_easy_perform failed: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        return "";
    }
    curl_easy_cleanup(curl);

    std::string verse_text;
    // Look for <meta property="og:description"> tag
    size_t pos = html.find("property=\"og:description\"");
    if (pos != std::string::npos) {
        size_t content_start = html.find("content=\"", pos) + 9; // Start after content="
        size_t content_end = html.find("\"", content_start);    // End at closing quote
        if (content_start != std::string::npos && content_end != std::string::npos) {
            verse_text = html.substr(content_start, content_end - content_start);
            verse_text = strip_tags(verse_text); // Clean any potential HTML entities
        }
    }

    if (verse_text.empty()) {
        std::cerr << "Error: No verse text found in HTML for reference: " << ref << std::endl;
        return "";
    }

    // Trim trailing spaces and newlines
    verse_text.erase(verse_text.find_last_not_of(" \n\r\t") + 1);

    return verse_text;
}

std::string get_executable_directory() {
#ifdef _WIN32
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string exe_path = path;
    // Find the last backslash to get the directory
    size_t last_slash = exe_path.find_last_of("\\");
    if (last_slash != std::string::npos) {
        return exe_path.substr(0, last_slash);
    }
    return "."; // Fallback to current directory if path extraction fails
#else
    // For non-Windows systems, use current directory as fallback
    return ".";
#endif
}