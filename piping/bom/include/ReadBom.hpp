#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <fstream>

class ReadBom {
public:

    struct Bom {
        std::string orig;    
        std::string size1;
        std::string size2;
        std::string desc;
        double size1_dec;
        double size2_dec;
        int indx_code;
        std::string grp;
        std::string short_desc;
    };

    std::vector<Bom> bom_lines;
    int GetData();

private:
    int get_input_type();
    int read_user_input(int);
    void separ_sizes_from_desc();
    int remove_lead_space_and_trailing_spaces(std::string& str);
    int remove_lead_space_or_x(std::string& str);
};