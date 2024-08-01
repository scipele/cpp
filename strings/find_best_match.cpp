#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

// Function to split a string into individual words
std::vector<std::string> splitString(const std::string& input) {
    std::vector<std::string> words;
    std::string word;
    for (char c : input) {
        if (std::isspace(c)) {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}

// Function to calculate the matching score for a phrase
int calculateMatchScore(const std::vector<std::string>& words1, const std::vector<std::string>& words2) {
    int score = 0;
    for (const auto& word : words1) {
        if (std::find(words2.begin(), words2.end(), word) != words2.end()) {
            score++;
        }
    }
    return score;
}

// Function to find the best matching phrase from a text file
std::string findBestMatch(const std::vector<std::string>& phrases, const std::string& input) {
    std::string bestMatch;
    int bestMatchCount = 0;

    for (const std::string& phrase : phrases) {
        // Split the phrase into individual words
        std::vector<std::string> words = splitString(phrase);

        // Count the number of occurrences of the input word in the phrase
        int matchCount = 0;
        for (const std::string& word : words) {
            if (word.find(input) != std::string::npos) {
                matchCount++;
            }
        }

        // Update the best match if the current phrase has more occurrences
        if (matchCount > bestMatchCount) {
            bestMatch = phrase;
            bestMatchCount = matchCount;
        }
    }

    return bestMatch;
}


int main() {
    std::cout << "Enter multiple words: ";
    std::string input;
    std::getline(std::cin, input);
    std::vector<std::string> inputWords = splitString(input);

    std::string filename = "C:\\Users\\mscip\\phrases.txt";
    std::string bestPhrase = findBestMatchingPhrase(inputWords, filename);

    if (!bestPhrase.empty()) {
        std::cout << "Best matching phrase: " << bestPhrase << std::endl;
    } else {
        std::cout << "No matching phrase found." << std::endl;
    }
 
    std::cout << "Press enter to exit...";
    std::cin.ignore(); // Wait for user input
    
    return 0;
}
