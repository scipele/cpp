#pragma once
#include <vector>

class FracConvert {
public:
    double FracToDecInch(const std::string& input);
    double FracToDecFeet(const std::string& input);
    double FracToDecMillimeter(const std::string& input);

private:
    struct ParsedData {
        std::string part;
        char type;  // defined delimiters f = feet delim, i = inch delim, p = decimal point
        double num;
    };
    std::vector<ParsedData> prsd;
    void parseInput(const std::string& input);
    std::string removeSubstringIgnoreCase(std::string str, const std::string& toRemove);
    void getParsedPartsOfInput(const std::string& input);
    void processStr(std::string& str);
    int getPatternNumber();
    double calcDecInch();
    double calcDecFeet();
    double calcDecMillimeter();
};