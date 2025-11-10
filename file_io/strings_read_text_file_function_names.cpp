#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>


// Function Prototypes
std::vector<std::string> extractFunctionsAndSubs(const std::string& filename);
void writeToFile(const std::string& outputFilename, const std::vector<std::string>& lines);


int main() {
   
    // get the file contents in a string
    std::string fullFileNamePath = "c:\\t\\TextRelated.vba";

    std::cout << fullFileNamePath;

    // extract the function and sub names
    std::vector<std::string> output = extractFunctionsAndSubs(fullFileNamePath);

    // sort the function and sub names
    std::sort(output.begin(), output.end());

    //call function write the Sub names to a text file
    std::string outputFilePath = "C:\\t\\sub_functions_list.txt";

    // Write sorted lines to the output file
    writeToFile(outputFilePath, output);
    
    return 0;
}


// Function to extract "Function" or "Sub" definitions up to the next parenthesis, ignoring "End Sub" or "End Function"
std::vector<std::string> extractFunctionsAndSubs(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open the file!");
    }

    std::vector<std::string> extractedLines;
    std::string line;

    while (std::getline(file, line)) {
        // Trim leading and trailing whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        // Ignore lines starting with "End"
        if (line.rfind("End", 0) == 0) { // Check if "End" is at the start
            continue;
        }

        // Find "Function" or "Sub"
        size_t functionPos = line.find("Function");
        size_t subPos = line.find("Sub");

        if (functionPos != std::string::npos || subPos != std::string::npos) {
            size_t keywordPos = (functionPos != std::string::npos) ? functionPos : subPos;
            std::string keyword = (functionPos != std::string::npos) ? "Function" : "Sub";

            // Extract the name following "Function" or "Sub"
            size_t nameStart = line.find_first_not_of(" \t", keywordPos + keyword.length());
            size_t nameEnd = line.find('(', nameStart);

            if (nameStart != std::string::npos && nameEnd != std::string::npos) {
                std::string name = line.substr(nameStart, nameEnd - nameStart);
                extractedLines.push_back(keyword + " " + name);
            }
        }
    }

    file.close();
    return extractedLines;
}



// Function to write a vector of strings to a file
void writeToFile(const std::string& outputFilename, const std::vector<std::string>& lines) {
    std::ofstream outFile(outputFilename);
    if (!outFile.is_open()) {
        throw std::runtime_error("Error: Could not open the output file!");
    }

    for (const auto& line : lines) {
        outFile << line << "\n";
        std::cout << line << std::endl;
    }

    outFile.close();
}