#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

double levenshteinDistance(const std::string& word1, const std::string& word2) {
    int m = word1.length();
    int n = word2.length();

    // Create a 2D matrix to store the distances
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    // Initialize the first row and column
    for (int i = 0; i <= m; ++i) {
        dp[i][0] = i;
    }

    for (int j = 0; j <= n; ++j) {
        dp[0][j] = j;
    }

    // Compute the distances
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (word1[i - 1] == word2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = 1 + std::min({ dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1] });
            }
        }
    }

    // The Levenshtein distance is the value in the bottom right corner of the matrix
    int levenshtein_distance = dp[m][n];

    // Compute the maximum length and normalize the distance
    int max_length = std::max(m, n);
    double norm_distance = 1.0 - (static_cast<double>(levenshtein_distance) / static_cast<double>(max_length));

    return norm_distance;
}

// This function reads a text file line by line
std::vector<std::string> readLinesFromFile(const std::string& filename) {
    std::vector<std::string> lines;
    std::string line;

    std::ifstream inputFile(filename);

    if (inputFile.is_open()) {
        while (std::getline(inputFile, line)) {
            lines.push_back(line);
        }
        inputFile.close();
    } else {
        std::cerr << "Failed to open the file: " << filename << std::endl;
    }

    return lines;
}

std::vector<std::vector<std::string>> splitLinesIntoWords(const std::vector<std::string>& lines) {
    std::vector<std::vector<std::string>> wordsArray;

    for (size_t i = 0; i < lines.size(); ++i) {
        std::vector<std::string> words;
        std::istringstream iss(lines[i]);
        std::string word;

        while (iss >> word) {
            words.push_back(word);
        }

        wordsArray.push_back(words);
    }

    return wordsArray;
}

void convertToUppercase(std::string& text) {
    for (char& c : text) {
        c = std::toupper(c);
    }
}

int main() {

    // 1. Read the following filename into dataArray
    std::string fileLib = "C:\\Users\\mscip\\documents\\tlkpLibrary.txt"; 
    std::vector<std::string> dataAryLib = readLinesFromFile(fileLib);

    // 2. Read the following filename into dataArray
    std::string fileLook = "C:\\Users\\mscip\\documents\\LookupList.txt"; 
    std::vector<std::string> dataAryLook = readLinesFromFile(fileLook);

    // 3. Convert all of the lines to uppercase
    for (std::string& line : dataAryLib) {
        convertToUppercase(line);
    }

    // 4. Convert all of the lines to uppercase
    for (std::string& line : dataAryLook) {
        convertToUppercase(line);
    }

    // 5. Next create a vector WordsArray from the original line items
    std::vector<std::vector<std::string>> wordsAryLib = splitLinesIntoWords(dataAryLib);
    std::vector<std::vector<std::string>> wordsAryLook = splitLinesIntoWords(dataAryLook);

    // 6. Begin looping thru each line and each word in both the Lookup Lines and Library
    //    Outer Loop fpr each Line of Lookup File

    double max_norm_dist = 0;
    double max_norm_dist_per_lookup_item = 0;
    int maxLine;
    int i;

    for (size_t i = 0; i < wordsAryLook.size(); ++i) {
        //Nested Loop for Each word in Lookup File
        for (const std::string& wordLook : wordsAryLook[i]) {
            //Nested Loop for each Line in the Library    
            
            // ***********************************************************************************
            // figure out how to swap loops later if the number of words in the Lib Line item is 
            // smaller than the current lookup line item
            // ***********************************************************************************
            for (size_t j = 0; j < wordsAryLib.size(); ++j) {
                //Nested Loop for each Word in Library
                max_norm_dist = 0;
                for (const std::string& wordLib : wordsAryLib[j]) {
                    double norm_dist = levenshteinDistance(wordLook, wordLib);
                    // Update max_norm_distance if necessary
                    if (norm_dist > max_norm_dist) {
                        max_norm_dist = norm_dist;
                    }
                }
                // Update max_norm_distance if necessary
                if ( max_norm_dist > max_norm_dist_per_lookup_item) {
                    max_norm_dist_per_lookup_item = max_norm_dist;
                    maxLine = j;
                }
            }
        }
    }
 
    std::cout << max_norm_dist << std::endl;
    std::cout << max_norm_dist_per_lookup_item << std::endl;
    std::cout << maxLine << std::endl;

    return 0;

}
