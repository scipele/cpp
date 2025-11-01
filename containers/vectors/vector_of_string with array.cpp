// filename:    vector_of_string with array.cpp
//
// Purpose:     This is an example showing how you can crete vector with an embedded array as the second dimension 
//              which in this case has three parts
//
// Dependencies:  None 
//
// By:T. Sciple 8/10/2024

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <array>

const size_t NUM_PARTS = 3;

// Function to split a line into a fixed-size array
void split_line_into_array(const std::string& line, std::array<std::string, NUM_PARTS>& parts) {
    std::istringstream stream(line);
    std::string token;
    size_t index = 0;
    
    // Extract parts using '|' delimiter
    while (std::getline(stream, token, '|') && index < NUM_PARTS) {
        parts[index++] = token;
    }
    
    // Handle the case where there are fewer parts than expected
    if (index < NUM_PARTS) {
        std::cerr << "\tWarning: Prev Line does not have " << NUM_PARTS << " parts.\n";
        while (index < NUM_PARTS) {
            parts[index++] = "";
        }
    }
}


int main() {
    std::vector<std::string> lines;
    lines.push_back("1_PartA|PartB|PartC");
    lines.push_back("2_PartA|PartB");
    lines.push_back("3_PartA|PartB|PartC");
    lines.push_back("4_PartA|PartB|PartC");

    std::vector<std::array<std::string, NUM_PARTS>> split_lines;
    
    //Pad screen with newline characters to clear up
    for (int i=0; i<25; i++) std::cout << "\n";

    // Declare a fixed-size array to hold the parts
    std::array<std::string, NUM_PARTS> parts;

    std::cout << "Here are the printed lines before splitting:\n";
    for (auto& line : lines) {
        std::cout << line << "\n";
        split_line_into_array(line, parts);
        split_lines.push_back(parts);
    }

    //loop thru each line and each part and print
    std::cout << "\n\nHere are the printed lines after splitting:\n";
    for (auto& line : split_lines) {
        for (auto& part : line) {
                std::cout << part << "  ";
        }
        std::cout << "\n";
    }
    std::cout << "\n\n";
    
    return 0;
}