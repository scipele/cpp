#include <iostream>
#include <string>
#include <cctype>

// Function to convert a string to lowercase
std::string lCase(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(c);
    }
    return result;
}

int main() {
    // Define a list of delimeters that will be checked thru until match is found
        // Define a list of delimiters that will be checked through until match is found
    std::string delims[8];
    
    // Initialize the array one element at a time for clarity of definition
    delims[0] = "ft"; 
    delims[1] = "f"; 
    delims[2] = "'";  // feet delimiters, dType 'f'
    delims[3] = "in"; 
    delims[4] = "i"; 
    delims[5] = "\""; 
    delims[6] = "''"; // inch delimiters, dType 'i'
    delims[7] = "/";  // division, dType 'd'
    
    // Get user input of string to check
    std::string chkStr;
    std::cout << "Enter a string: ";
    std::getline(std::cin,  chkStr);
    chkStr = lCase(chkStr);

    int index=0;
    bool isMatch;
    do {
        if(chkStr == delims[index]) {
            isMatch = true;
            break; // Exit the loop if a match is found
        }
        index += 1;
    } while (index<8);

    char dType;
    // Switch based on the value of index
    switch (index) {
        case 0 ... 2:
            dType = 'f'; // feet
            break;
        case 3 ... 6:
            dType = 'i'; // inch
            break;
        case 7:
            dType = 'd'; // fracDiv
            break;
        default:
            dType = 'n'; // not found
    }
    std::cout << "index:" << index << ", Type: " << dType;

    return 0;
}
