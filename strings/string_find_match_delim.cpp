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

char getDelimType(std::string str) {
    // Define an array list of delimiters that will be checked through until a match is found
    std::pair<std::string, char> delims[10] = {
        { "ft",     'f'},   // 0    feet delimiters, dType 'f'
        { "f",      'f'},   // 1    ditto
        { "'",      'f'},   // 2    ditto
        { "feet",   'f'},   // 3    feet delimiters, dType 'f'
        { "in",     'i'},   // 4    inch delimiters, dType 'i'
        { "i",      'i'},   // 5    ditto
        { "inch",   'i'},   // 6    inch delimiters, dType 'i'
        { "\"",     'i'},   // 7    ditto
        { "''",     'i'},   // 8    ditto
        { "/",      'd'},   // 9    division, dType 'd'
    };
    
    for(int i = 0; i <= 7; i++) {
        if(str.compare(delims[i].first) == 0) {
            std::cout << "Index:" << i;
            return delims[i].second;
        }
    }
    return 'n';  // return 'n' if match is not found
}


int main() {
    
    // Get user input of string to check
    std::string chkStr;
    std::cout << "Enter a string: ";
    std::getline(std::cin,  chkStr);
    chkStr = lCase(chkStr);

    char dType = getDelimType(chkStr);
    
    std::cout << "\nType: " << dType;

    return 0;
}
