#pragma once
#include "Bom.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <fstream>

class ReadBom {
public:
    std::vector<Bom> GetData();

private:
    int get_input_type();
    int read_user_input(int, std::vector<Bom>& lines);
    void separ_sizes_from_desc (std::vector<Bom>& lines);
    int remove_lead_space_and_trailing_spaces(std::string& str);
    int remove_lead_space_or_x(std::string& str);
};