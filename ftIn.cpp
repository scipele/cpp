#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <iomanip>

// Function to pause the console window until a key is pressed
void pauseConsole() {
    std::cout << "Press any key to continue...";
    std::string input;
    std::getline(std::cin, input); // Wait for the user to press Enter
}

char getDelimType(std::string str) {
    // Define a list of delimiters that will be checked through until a match is found
    std::pair<std::string, char> delims[8] = {
        { "ft", 'f'},   // 0    feet delimiters, dType 'f'
        {  "f", 'f'},   // 1    ditto
        {  "'", 'f'},   // 2    ditto
        { "in", 'i'},   // 3    inch delimiters, dType 'i'
        { "i",  'i'},   // 4    ditto
        {  "\"",'i'},   // 5    ditto
        { "''", 'i'},   // 6    ditto
        { "/",  'd'},   // 7    division, dType 'd'
    };
    
    for(int i = 0; i <= 7; i++) {
        if(str.compare(delims[i].first) == 0) {
            return delims[i].second;
        }
    }
    return 'n';  // return 'n' if match is not found
}

std::vector<std::pair<int, int>> getNumsInStr(const std::string& str) {
    std::vector<std::pair<int, int>> result; // Vector of pairs to store index position and number
    std::string num_str;                      // String to temporarily hold consecutive numeric digits
    int index = 0;                            // Index position
    
    for (char c : str) {
        if (std::isdigit(c)) {
            num_str += c;          // If this char is a number, append it to num_str
        } else if (!num_str.empty()) {
            // If num_str is not empty, convert it to an integer and add it to the result vector
            result.push_back(std::make_pair(index - num_str.length(), std::stoi(num_str)));
            num_str.clear();      // Clear num_str for the next sequence of numeric digits
        }
        ++index;                 // Increment index position
    }
    
    // Check if there are any remaining digits in num_str after the loop
    if (!num_str.empty()) {
        result.push_back(std::make_pair(index - num_str.length(), std::stoi(num_str)));
    }

    return result;
}


std::vector<std::pair<int, std::string>> getConsecutiveDelims(const std::string& str) {
    std::vector<std::pair<int, std::string>> result;  // Vector of pairs to store index position and delimiter
    std::string del_str;                              // String to temporarily hold consecutive delimiters
    int index = 0;                                    // Index position
    
    // Define all the potential delimiters that could represent feet, inches, division 
    std::string potential_delims = "ft\'in\"'\"/";
    for (char c : str) {
        if (potential_delims.find(c) != std::string::npos) {
            del_str += c;          // If this char is a delimiter, append it to del_str
        } else if (!del_str.empty()) {
            // If del_str is not empty, add it to the result vector
            result.push_back(std::make_pair(index - del_str.length(), del_str));
            del_str.clear();      // Clear del_str for the next sequence of delimiters that match specified group
        }
        ++index;                 // Increment index position
    }
    
    // Check if there are any remaining delimiters in del_str after the loop
    if (!del_str.empty()) {
        result.push_back(std::make_pair(index - del_str.length(), del_str));
    }

    return result;
}

struct data {
    int loc;    
    int num;
    char basic_type;
    char delim_type;
};

// Comparator function to sort Data objects by index
bool compareByLoc(const data& a, const data& b) {
    return a.loc < b.loc;
}

int main() {

// Hardcode in feet -inches - sixteeths
int ft_whole=0;
int in_whole=0;
int in_numerator=0;
int in_denominator=0;

std::cout << "Enter Ft, Inches, and Fraction in form similar to 3'-7 15/16: ";
std::string input;
std:getline(std::cin, input); 

    // 1. Call this Function to get consecutive numbers in the string
    std::vector<std::pair<int, int>> result = getNumsInStr(input);

    // Create an empty vector of data structs
    std::vector<data> structVector;

    int j = 0;

    for (auto& pair : result) {
        //user for testing std::cout << "Index: " << pair.first << ", Number: " << pair.second << std::endl;
        // Create a new instance of the data struct
        struct data newData;
        newData.loc = pair.first;
        newData.num = pair.second;
        newData.basic_type = 'n';
        newData.delim_type = '-';
        // Add the new instance to the vector
        structVector.push_back(newData);
        j += 1;
    }

    // 3. Print the index position and the corresponding delim extracted from the string
    std::vector<std::pair<int, std::string>> result2 = getConsecutiveDelims(input);
    for (auto& pair : result2) {
        char del_type= getDelimType(pair.second);
        //Used for testing,  std::cout << "Index: " << pair.first << ", Delimeter: " << pair.second << ", Type: " << del_type << std::endl;
        struct data newData;
        newData.loc = pair.first;
        newData.num = 0;        
        newData.basic_type = 'd';
        newData.delim_type = del_type; // not used
        // Add the new instance to the vector
        structVector.push_back(newData);
        j += 1;
    }

    // Sort the vector based on the loc member using the custom comparison function
    std::sort(structVector.begin(), structVector.end(), compareByLoc);

    //print the vector sorted by loc
    j = 0;
    for (const auto& item : structVector) {
        // used for testing,  std::cout << "i: " << i << "\tloc: " << item.loc << "\t\t" << "num: " << item.num << "  \tbasic_type: " << item.basic_type << "\t" "delim_type: " << item.delim_type << "\n";
        j += 1;  //determines the count for array sizing
    }

    const int ARY_SZ = j;
    
    /* read data into a structure array now that the size is known
        so that the previous element is easy access */
    data dAry[ARY_SZ];
    j = 0;

    for (const auto& item : structVector) {
        dAry[j].loc = item.loc;
        dAry[j].num = item.num;
        dAry[j].basic_type = item.basic_type;
        dAry[j].delim_type = item.delim_type;
        j++;
    }
    for (j = 0; j <= ARY_SZ; j++) {
        if(j>=1) {
            if(dAry[j].basic_type == 'd') {
                if(dAry[j-1].basic_type == 'n') {
                    switch (dAry[j].delim_type)
                    {
                    case 'f':
                        ft_whole = dAry[j-1].num;
                        break;
                    
                    case 'd':
                        if(dAry[j-2].basic_type =='n') {
                            in_whole = dAry[j-2].num;
                        }
                        in_numerator = dAry[j-1].num;
                        in_denominator = dAry[j+1].num;                        
                        break;
                    case 'i':
                        if (in_whole == 0) {
                            in_whole = dAry[j-1].num;
                        }
                    default:
                        break;
                    }
                }
            }
        }
    }

    //convert to Dec Feet
    double dec_ft;
    double in_frac;
    if(in_denominator == 0) {
        in_frac = 0;
    } else {
        in_frac = (static_cast<double>(in_numerator))/(static_cast<double>(in_denominator));
    }

    dec_ft = static_cast<double>(ft_whole) + (static_cast<double>(in_whole) + in_frac)/12;

    std::cout << "Input:\t" << input << "\tdec ft:\t" << std::setprecision(20) << dec_ft << "\n";

    pauseConsole();

return 0;
}

/*  Used this for testing
std::string inputAry[5] = { 
    {"132'-7 3/16\"" },
    {"20' 3 7/8\"" },
    {"2'-4\"" },
    {"7 3/16\""},
    {"63ft 8-9/16in"}
};
    for (int i = 0; i <= 4; i++) {
    input = inputAry[i];
    //reset variables to zero
    ft_whole=0; in_whole=0; in_numerator=0; in_denominator=0;
*/



