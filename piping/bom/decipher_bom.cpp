// filename:  decipher_bom.cpp
//
// Purpose:
//   1. Read bom description lines from a text file (each line as separate item)
//   2. Separate Size Information from the description into (sz_1, sz_2, desc) if requried
//   3. Categories each line into the following commodity coding system
//      AJ|XX|X|0001|AB|AB|11|00|A|X
//      |  |  |  |   |  |  |  |  | |
//      |  |  |  |   |  |  |  |  | |
//      |  |  |  |   |  |  |  |  | +---- end_typ_redc
//      |  |  |  |   |  |  |  |  +---- end_typ_main
//      |  |  |  |   |  |  |  +---- sch_redc
//      |  |  |  |   |  |  +---- sch_main
//      |  |  |  |   |  +---- matl_grade
//      |  |  |  |   +--- matl_code
//      |  |  |  +---- indx_code
//      |  |  +---- rtg
//      |  +---- size_redc
//      +---- size_main
//
//  4. Output file is generated that contains the following pipe ("|") delimited items
//      sz_1 | sz_2 | original_desc | standardized description | commodity code
//
//
// Dependencies:  None
//
// By:  T.Sciple, 8/9/2024

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cctype>  //Provides functions for character handling, such as std::tolower.
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>

class ReadBom {
public:

    struct Bom {
        std::string orig;    
        std::string size1;
        std::string size2;
        std::string desc;
        double size1_dec;
        double size2_dec;
    };

    std::vector<Bom> GetData() {
        // Step 1. Determine from user if size information needs to be split
        int input_type = get_input_type();
        std::cout << input_type << "\n";

        // Step 2. read user input depending on type
        std::vector<Bom> lines;
        int result = read_user_input(input_type, lines);

        // Step 3 call splitter function
        separ_sizes_from_desc(lines);

        return lines;
    }


private:
    // Step 1
    int get_input_type() {
        for (int i=0; i<25; i++) std::cout << "\n";
        std::cout   << "create a text file named and located in c:\\t\\bom\\input.txt\n\n"
                    << "Input 1, 2, or 3 depending on how the input file is organized:\n"
                    << "1. Already separated with a pipe delimeter \"|\" by size1 | size 2 | desc\n"
                    << "2. Separated with a pipe delimeter \"|\" by size1&2 | desc\n"
                    << "3. Sizes and Description is merged together\n"
                    << "User Enter (1, 2, or 3) -> ";
        
        int type;
        //********************************************************************************************************************
        // ToDo - Finish other cases - hardcode as 3 for now *****************************************************************
        //********************************************************************************************************************
        // std::cin >> type;
        type = 3;

        if(type > 0 && type < 4) {
            return type;
        } else {
            std::cout << "Invalid Entry - Run Again\n";
            return -1;
        }
    }

    // Step 2 read input
    int read_user_input(int, std::vector<Bom>& lines) {

        // open file
        std::ifstream file("C:/t/bom/input.txt");

        // check if the file opened successfully
        if (!file) {
            std::cerr << "Unable to open file: " << std::endl;
            return -1;  // return -1 iff error reading the file
        }

        std::string line;
        while (std::getline(file, line)) {
            lines.push_back( { line , "", "", "", _BLANK , _BLANK });
            //lines.push_back( { line , "", "", "", 0.0, 0.0 });
        }

        file.close();
        return 0;
    }

    // Step 3 separate sizes from desc
    void separ_sizes_from_desc (std::vector<Bom>& lines) {
        
        int indx=1;
        for (auto& line : lines) {
            int loc=0;
            int offset;
            int len = line.orig.size();
            char c; 
            for (int i=0; i<len; i++) {
                offset = 0;
                c = tolower(line.orig[i]);
                if( c == '\"' ) {   // handle the case where there is a missing inch mark
                    if(loc == 0) {
                        line.size1 = line.orig.substr(0, i+1);
                        offset = remove_lead_space_and_trailing_spaces(line.size1);
                        loc = i + 1;
                    } else {
                        line.size2 = line.orig.substr(loc, i-loc+1+offset);
                        offset = offset + remove_lead_space_and_trailing_spaces(line.size2);
                        offset = offset + remove_lead_space_or_x(line.size2);
                        loc = i + 1;
                        break;
                    } 
                } 
                // Assume Size 1 and or Size 2 are located within the first 12 characters
                if(i > 11) break; 
            }
            line.desc = line.orig.substr(loc, len-loc);   // try -1
            remove_lead_space_and_trailing_spaces(line.desc);
            indx++;

            /* Temporary print for troubleshooting
            std::cout   << indx << ". |"
                        << line.orig << "|"
                        << line.size1 << "|"
                        << line.size2 << "|"
                        << line.desc << "|"
                        << line.size1_dec << "|"
                        << line.size2_dec << "\n"; */
        }
    }


    int remove_lead_space_and_trailing_spaces(std::string& str) {
        //trim any leading spaces
        int prev_len = str.length();
        char c;
        for (int i=0; i<str.length(); i++) {
            c = tolower(str[i]);
            if(c != ' ') {
                str = str.substr(i,str.length()-i);
                break;
            }
        }
        //trim any trailing spaces
        int start_pos = str.length()-1;
        for (int i=start_pos; i>0; i--) {
            c = str[i]; 
            if(c != ' ') {
                str = str.substr(0,i+1);
                break;
            }
        }
        int final_len = str.length();
        return (prev_len - final_len);
    }


    int remove_lead_space_or_x(std::string& str) {
        int prev_len = str.length();
        //trim any leading spaces or x or X
        char c;
        for (int i=0; i<str.length(); i++) {
            c = tolower(str[i]);
            if(c != 'x' && c != ' ') {
                str = str.substr(i,str.length()-i);
                break;
            }
        }
        int final_len = str.length();
        return (prev_len - final_len);
    }
};


class FracConvert {
public:
    int ConvertSizesToDec(std::vector<ReadBom::Bom>& bm) {
        int i=1;
        for ( auto& b : bm ) {
            try
            {
                b.size1_dec = FracToDecInch(b.size1);
                b.size2_dec = FracToDecInch(b.size2);
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
                                    
            // used for testing
            std::cout << i << ". sz1 = " << b.size1 << "|" << "sz2 = " << b.size2 << "\n";
            i++;
        }
        return 0;
    }

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
        char type;  // defined delimiters f = feet delim, i = inch delim, p = decimal point
        double num;
    };

    std::vector<ParsedData> prsd;

    void parseInput(const std::string& input) {
        prsd.clear();  // Clear previous parsed data
        getParsedPartsOfInput(input);
    }

    std::string removeSubstringIgnoreCase(std::string str, const std::string& toRemove) {
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

    void getParsedPartsOfInput(const std::string& input) {
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

    void processStr(std::string& str) {
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

    int getPatternNumber() {
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

    double calcDecInch() {
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

    double calcDecFeet() {
        return calcDecInch() / 12.0;
    }

    double calcDecMillimeter() {
        return calcDecInch() * 25.4;
    }
};


int main(int argc, char const *argv[])
{
     // Record start time
    auto start = std::chrono::high_resolution_clock::now();

    //instantiate class as rb
    ReadBom rb;
    //create a vector of the class
    std::vector<ReadBom::Bom> bm;
    bm = rb.GetData();
    
    // Convert Sizes to Decimal
    FracConvert conv;
    int result = conv.ConvertSizesToDec(bm);

    //Temp Print out
    int i=1;
    for(auto& line : bm) {
        std::cout   << i << ". |"
                    << line.orig << "|"
                    << line.size1 << "|"
                    << line.size2 << "|"
                    << line.desc << "|"
                    << line.size1_dec << "|"
                    << line.size2_dec << "\n";
        i++;
    }


    // Record end time
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    std::chrono::duration<double> duration = end - start;

    // Output the duration
    std::cout << "\n\nProcessed " << bm.size() << " lines in " << duration.count() << " seconds" << std::endl;
    
    // Pause the console window before exiting
    system("pause");

    return 0;
}