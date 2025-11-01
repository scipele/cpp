#include "../../include/ReadBom.hpp"
#include <iostream>
#include <fstream>
#include <vector>

int ReadBom::GetData() {
    // Step 1. Determine from user if size information needs to be split
    int input_type = get_input_type();
    //std::cout << input_type << "\n";

    // Step 2. read user input depending on type
    int result = read_user_input(input_type);

    // Step 3 call splitter function
    separ_sizes_from_desc();

    return 0;
}

// Step 1
int ReadBom::get_input_type() {
    for (int i=0; i<25; i++) std::cout << "\n";
    std::cout   << "create a text file named and located in c:\\t\\bom\\input.csv\n\n"
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
int ReadBom::read_user_input(int) {

    // open file
    std::ifstream file("../data/input/input.csv");

    // check if the file opened successfully
    if (!file) {
        std::cerr << "Unable to open file: " << std::endl;
        return -1;  // return -1 iff error reading the file
    }

    std::string line;
    while (std::getline(file, line)) {
        this->bom_lines.push_back( { line , "", "", "", _BLANK , _BLANK, 0, "", "" });
    }

    file.close();
    return 0;
}

// Step 3 separate sizes from desc
void ReadBom::separ_sizes_from_desc() {
    int indx=1;
    for (auto& line : bom_lines) {
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

int ReadBom::remove_lead_space_and_trailing_spaces(std::string& str) {
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

int ReadBom::remove_lead_space_or_x(std::string& str) {
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