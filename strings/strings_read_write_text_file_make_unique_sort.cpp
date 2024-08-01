#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

// Function Prototypes
std::vector<std::string> split(const std::string& str, char delimiter);
std::string join(const std::vector<std::string>& vec, char delimiter);
void writeStringsToFile(std::string& output);
std::string getStringFromTextFile();


int main() {
    
    // get the file contents in a string
    std::string input = getStringFromTextFile();

    // Split the input string into a vector of strings
    std::vector<std::string> strings = split(input, '\n');

    // Sort the vector of strings
    std::sort(strings.begin(), strings.end());

    // Remove duplicates
    auto last = std::unique(strings.begin(), strings.end());
    strings.erase(last, strings.end());

    // Join the sorted strings back into a single string
    std::string output = join(strings, '\n');

    //call function to output the sorted unique strings to text file
    writeStringsToFile(output);

    // Print the sorted string and write it back to a file
    std::cout << output << std::endl;

    return 0;
}


// Function to split a string by a delimiter and return a vector of strings
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to join a vector of strings into a single string with a delimiter
std::string join(const std::vector<std::string>& vec, char delimiter) {
    std::string result;
    for (size_t i = 0; i < vec.size(); ++i) {
        result += vec[i];
        if (i < vec.size() - 1) {
            result += delimiter;
        }
    }
    return result;
}

//function to write the string to a file
// Optional: Write the sorted strings back to a file
void writeStringsToFile(std::string& output) {
    std::string outputFilePath = "C:\\t\\sorted_unique_list.txt";
    std::ofstream outputFile(outputFilePath);
    if (outputFile) {
        outputFile << output;
        outputFile.close();
    } else {
        std::cerr << "Unable to open file for writing: " << outputFilePath << std::endl;
    }
}

std::string getStringFromTextFile() {
// Path to the input file
    std::string inputFilePath = "C:\\t\\list.txt";
    std::ifstream inputFile(inputFilePath);

    // Check if the file opened successfully
    if (!inputFile) {
        std::cerr << "Unable to open file: " << inputFilePath << std::endl;
    }

    // Read the entire file into a string
    std::string input((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>()); 
    inputFile.close();
return input;
}