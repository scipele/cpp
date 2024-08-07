#include <iostream>
#include <string>
#include <vector>
#include <map>

class FracConvert {
public:
    double FracToDecInch(const std::string& input) {
        parseInput(input);
        return calcDecInch();
    }

    double FracToDecFeet(const std::string& input) {
        parseInput(input);
        return calcDecFeet();
    }

    double FracToDecMillimeter(const std::string& input) {
        parseInput(input);
        return calcDecMillimeter();
    }

private:
    struct ParsedData {
        std::string part;
        char type;  // defined delimiters f = feet delim, i = inch delim
        int num;
    };

    std::vector<ParsedData> prsd;

    void parseInput(const std::string& input) {
        prsd.clear();  // Clear previous parsed data
        getParsedPartsOfInput(input);
    }

    void getParsedPartsOfInput(const std::string& input) {
        std::string str;
        bool is_prev_num = false;

        for (auto it = input.begin(); it < input.end(); it++) {
            char c = tolower(*it);  // set c = lowercase of current iterator
            int case_no = (std::isdigit(c) == is_prev_num) ? 1 : 2;  //  1 = same as previous,  2 = different type
            if (c == ' ' || c == '-') case_no = 3;  //  3 = blank or dash
            if ((it == input.end() - 1) && case_no == 1) case_no = 4;  //  4 = at end, but same type as previous
            if ((it == input.end() - 1) && case_no == 2) case_no = 5;  //  5 = at end, but different type as previous

            switch (case_no) {
                case 1:  //  1 = same type as previous
                    str += c;  // Concatenate w/ prev
                    break;
                case 2:  // 2 = different type than previous
                    if (str != "") processStr(str);  // process prev
                    str = c;  // starts is_new str
                    is_prev_num = std::isdigit(c) ? true : false;
                    break;
                case 3:  //  3 = blank or dash
                    if (str != "") processStr(str);
                    is_prev_num = false;
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

    void processStr(std::string& str) {
        int num = 0;
        char type;
        if (isdigit(str[0])) {
            type = 'n';
            num = std::stoi(str);
        } else {
            std::map<std::string, char> delims = {
                {"ft", 'f'}, {"feet", 'f'}, {"f", 'f'}, {"'", 'f'},  // feet delims map to type 'f'
                {"in", 'i'}, {"inch", 'i'}, {"i", 'i'}, {"\"", 'i'}, {"''", 'i'},  // inch delimiters, map to type 'i'
                {"/", 'd'}  // division, map to type 'd'
            };
            type = delims[str];  // set the current type based on the key supplied from the map above
        }
        prsd.push_back({str, type, num});
        str.clear();  // clear content of string
    }

    int getPatternNumber() {
        std::string ptrn_key;
        for (auto p : prsd) ptrn_key += p.type;

        std::map<std::string, int> pattern = {
            {"nfnndni", 1}, {"nfnndn", 1},      // 32'-5 7/16" = 32.4531, second assumes missing inch at end
            {"nfndni", 2}, {"nfndn", 2},        // 12' 9/16" = 12.0469, second assumes missing inch at end
            {"nfni", 3}, {"nfn", 3},            // 32'-5" = 32.4167, second assumes missing inch at end
            {"nndni", 4}, {"nndn", 4},          // 5 7/16" = 0.4531, second assumes missing inch at end
            {"ni", 5},                          // 5" = 0.4167
            {"nf", 6}                           // 14' = 14.0
        };
        return pattern[ptrn_key];  // set the current type based on the key supplied from the map above
    }

    double calcDecInch() {
        int pattern = getPatternNumber();
        switch (pattern) {
            case 1: return static_cast<double>(prsd[0].num) * 12.0 + (static_cast<double>(prsd[2].num) + static_cast<double>(prsd[3].num) / static_cast<double>(prsd[5].num));
            case 2: return static_cast<double>(prsd[0].num) * 12.0 + (static_cast<double>(prsd[2].num) / static_cast<double>(prsd[4].num));
            case 3: return static_cast<double>(prsd[0].num) * 12.0 + static_cast<double>(prsd[2].num);
            case 4: return (static_cast<double>(prsd[0].num) + static_cast<double>(prsd[1].num) / static_cast<double>(prsd[3].num));
            case 5: return static_cast<double>(prsd[0].num);
            case 6: return static_cast<double>(prsd[0].num) *12.0;
            default: return 0;
        }
    }

    double calcDecFeet() {
        return calcDecInch() / 12.0;
    }

    double calcDecMillimeter() {
        return calcDecInch() * 25.4;
    }
};

int main() {
 
    FracConvert conv;
    std::string input;

    std::cout << "\nConvert Fractional Feet, Inches and Fractions to Decimal (ie 43'-7 7/16\") : ";
    std::getline(std::cin, input);

    std::cout << "Decimal Feet = " << conv.FracToDecFeet(input) << "\n";
    std::cout << "Decimal Inch = " << conv.FracToDecInch(input) << "\n";
    std::cout << "Millimeters = " << conv.FracToDecMillimeter(input) << "\n";  
}
