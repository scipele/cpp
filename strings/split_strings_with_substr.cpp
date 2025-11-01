#include <iostream>
#include <string>
#include <vector>

struct bom {
    std::string orig;    
    std::string size1;
    std::string size2;
    std::string desc;
};

// Function Prototypes
void separ_sizes_from_desc (std::vector<bom>& lines);
void remove_lead_space_and_trailing_spaces(std::string& str);
void remove_lead_space_or_x(std::string& str);


int main() {

    // hard code a few lines as an example
    std::vector<bom> lines;
    lines.push_back( { "10\" x 4\" WOL A105 CS S/STD BORE  ", "", "", "" } );
    lines.push_back( { "10\" PIPE CS S/STD BORE  ", "", "", "" } );
    lines.push_back( { "PIPE HANGER FOR 1\" CS PIPE  WELDED (TYPE H-1-137)", "", "", "" } );
    lines.push_back( { "10\" x 4\"", "", "", "" } );

    //Call the separ_sizes_from_desc for each line
    separ_sizes_from_desc(lines);

    //Print results of the parsing operations
    for (int i=0; i<50; i++) std::cout << "\n"; // print several line feeds to clear out screen
    std::cout << "Split string parts\n\n";

    for (auto& line : lines) {
        std::cout << std::left << line.size1 << "|";
        std::cout << std::left << line.size2 << "|";
        std::cout << std::left << line.desc << "|";
        std::cout << "\n";
    }
    std::cout << std::endl;
    return 0;
}


void separ_sizes_from_desc (std::vector<bom>& lines) {
    for (auto& line : lines) {
        int loc=0;
        int len = line.orig.size();
        for (int i=0; i<len; i++) {
            if(line.orig[i] == '\"') {
                if(loc == 0) {
                    line.size1 = line.orig.substr(0, i+1);
                    remove_lead_space_and_trailing_spaces(line.size1);
                    loc = i + 1;
                } else {
                    line.size2 = line.orig.substr(loc, i-loc+1);
                    remove_lead_space_and_trailing_spaces(line.size2);
                    remove_lead_space_or_x(line.size2);
                    loc = i + 1;
                    break;
                }
            } 
            // Assume Size 1 and or Size 2 are located within the first 12 characters
            if(i > 11) break; 
        }
        line.desc = line.orig.substr(loc, len-loc);   // try -1
        remove_lead_space_and_trailing_spaces(line.desc);
    }
}


void remove_lead_space_and_trailing_spaces(std::string& str) {
    //trim any leading spaces
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
}


void remove_lead_space_or_x(std::string& str) {
    //trim any leading spaces or x or X
    char c;
    for (int i=0; i<str.length(); i++) {
        c = tolower(str[i]);
        if(c != 'x' && c != ' ') {
            str = str.substr(i,str.length()-i);
            break;
        }
    }
}