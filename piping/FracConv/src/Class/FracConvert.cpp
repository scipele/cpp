// Filename:    FracConvert.cpp
// 
// Purpose:     Converts fractional measurements to decimal feet, inches, or millimeters
// 
// by:          JAS, 10/5/2024

#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include "../../include/FracConvert.hpp"

double FracConvert::FracToDecInch(const std::string& input) {
    parseInput(input);
    return calcDecInch();
}

double FracConvert::FracToDecFeet(const std::string& input) {
    parseInput(input);
    return calcDecFeet();
}

double FracConvert::FracToDecMillimeter(const std::string& input) {
    parseInput(input);
    return calcDecMillimeter();
}

//std::vector<ParsedData> prsd;

void FracConvert::parseInput(const std::string& input) {
    prsd.clear();  // Clear previous parsed data
    getParsedPartsOfInput(input);
}

std::string FracConvert::removeSubstringIgnoreCase(std::string str, const std::string& toRemove) {
    // Convert both strings to lowercase
    std::string strLower = str;
    std::string toRemoveLower = toRemove;
    
    std::transform(strLower.begin(), strLower.end(), strLower.begin(), ::tolower);
    std::transform(toRemoveLower.begin(), toRemoveLower.end(), toRemoveLower.begin(), ::tolower);
    
    // Find the position of the substring in the lowercase string
    size_t pos = strLower.find(toRemoveLower);
    
    // Remove the substring if found
    if (pos != std::string::npos) {
        str.erase(pos, toRemove.length());
    }
    return str;
}

void FracConvert::getParsedPartsOfInput(const std::string& input) {
    std::string str;
    bool is_prev_num = false;
    bool is_prev_blank = false;

    // Handle the case that contains 'flat x' for say an olet
    removeSubstringIgnoreCase(str, "flat x");

    for (auto it = input.begin(); it < input.end(); it++) {
        char c = tolower(*it);                                      // set c = lowercase of current iterator
        int case_no = (std::isdigit(c) == is_prev_num ) ? 1 : 2;    //  1 = same as previous,  2 = different type
        if ( (c == '.') ) case_no = 1; is_prev_num = true;          //  1 = consider decimal point part of num 
        if( is_prev_blank ) case_no = 2; is_prev_blank = false;     //  2 = different type if previous was blank, set back to false
        if ( c == ' ' || c == '-' ) case_no = 3;                    //  3 = blank or dash
        if ( (it == input.end() - 1) && case_no == 1 ) case_no = 4; //  4 = at end, but same type as previous
        if ( (it == input.end() - 1) && case_no == 2 ) case_no = 5; //  5 = at end, but different type as previous

        // used for testing
        //std::cout << "c = " << c << ", case = " << case_no << "\n";
        switch (case_no) {
            case 1:  //  1 = same type as previous
                str += c;  // Concatenate w/ prev
                break;
            case 2:  // 2 = different type than previous
                if ( (str != "") && (str != " ") ) {
                    processStr(str);  // process prev
                }
                str = c;  // starts is_new str
                is_prev_num = std::isdigit(c) ? true : false;
                break;
            case 3:  //  3 = blank or dash
                if ( str != "" ) processStr(str);
                is_prev_num = false;
                is_prev_blank = true;
                break;
            case 4:  //  4 = at end, but same type as previous
                str += c;  // Concatenate w/ prev
                processStr(str);
                break;
            case 5:  //  5 = at end, but different type as previous
                processStr(str);  // Process Previous String
                str = c;  // Process as New String
                processStr(str);  // Process New String
                break;
        }
    }
}

void FracConvert::processStr(std::string& str) {
    double num = 0;
    char type;
    if (isdigit(str[0]) || str[0] == '.') {
        type = 'n';
        num = std::stod(str);
    } else {
        std::map<std::string, char> delims = {
            {"ft", 'f'}, {"feet", 'f'}, {"f", 'f'}, {"'", 'f'},  // .............. feet delims map to type 'f'
            {"in", 'i'}, {"inch", 'i'}, {"i", 'i'}, {"\"", 'i'}, {"''", 'i'}, //.. inch delimiters, map to type 'i'
            {"/", 'd'},  // ....................................................... division, map to type 'd'
            {".", 'p'}  // ....................................................... decimal point, map to type 'p'
        };
        type = delims[str];  // set the current type based on the key supplied from the map above
    }
    prsd.push_back({str, type, num});
    // used for testing
    //std::cout << "str = " << str << "\n";
    str.clear();  // clear content of string
}

int FracConvert::getPatternNumber() {
    std::string ptrn_key;
    for (auto p : prsd) ptrn_key += p.type;

    std::map<std::string, int> pattern = {
        {"nfnndni", 1}, {"nfnndn", 1},  // 32'-5 7/16" = 32.4531 feet, second assumes missing inch at end
        {"nfndni", 2}, {"nfndn", 2},    // 12' 9/16" = 12.0469 feet, second assumes missing inch at end
        {"nfni", 3}, {"nfn", 3},        // 32'-5" = 32.4167 fett, second assumes missing inch at end
        {"nndni", 4}, {"nndn", 4},      // 5 7/16" = 0.4531 feet, second assumes missing inch at end
        {"ndni", 5}, {"ndn", 5},        // 5/8" = 0.625 inches, second assumes missing inch at end
        {"ni", 6}, {"n", 6},            // .75 = .75 inches are assumed
        {"nf", 7}                       // 14' = 14.0
    };
    return pattern[ptrn_key];  // set the current type based on the key supplied from the map above
}

double FracConvert::calcDecInch() {
    int pattern = getPatternNumber();
    switch (pattern) {
        case 1: return static_cast<double>(prsd[0].num) * 12.0 + (static_cast<double>(prsd[2].num) + static_cast<double>(prsd[3].num) / static_cast<double>(prsd[5].num));
        case 2: return static_cast<double>(prsd[0].num) * 12.0 + (static_cast<double>(prsd[2].num) / static_cast<double>(prsd[4].num));
        case 3: return static_cast<double>(prsd[0].num) * 12.0 + static_cast<double>(prsd[2].num);
        case 4: return (static_cast<double>(prsd[0].num) + static_cast<double>(prsd[1].num) / static_cast<double>(prsd[3].num));
        case 5: return static_cast<double>(prsd[0].num) / static_cast<double>(prsd[2].num);
        case 6: return static_cast<double>(prsd[0].num);
        case 7: return static_cast<double>(prsd[0].num) * 12.0;
        default: return 0;
    }
}

double FracConvert::calcDecFeet() {
    return calcDecInch() / 12.0;
}

double FracConvert::calcDecMillimeter() {
    return calcDecInch() * 25.4;
}