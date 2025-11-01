#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Define the struct
struct categ_data {
    int cat_id;
    std::string grp;
    std::string short_desc;
    std::vector<std::string> incl_all;
    std::vector<std::string> incl_any;
    std::vector<std::string> incl_none;
};

// Helper function to split a string by a delimiter
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(str);
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Helper function to parse the input line into a struct
categ_data parse_line(const std::string& line) {
    categ_data data;
    std::stringstream ss(line);
    std::string field;          // temp string to hold field data

    // Read cat_id
    std::getline(ss, field, ';');
    data.cat_id = std::stoi(field);

    // Read grp
    std::getline(ss, data.grp, ';');

    // Read short_desc
    std::getline(ss, data.short_desc, ';');

    // Read incl_all
    std::getline(ss, field, ';');
    data.incl_all = split(field, '|');

    // Read incl_any
    std::getline(ss, field, ';');
    data.incl_any = split(field, '|');

    // Read incl_none
    std::getline(ss, field, ';');
    data.incl_none = split(field, '|');

    return data;
}

int main() {
    std::ifstream file("c:/dev/cpp/piping/bom/cat_def.csv"); // Open your CSV file

    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    std::vector<categ_data> data_vector; // Vector to store parsed data
    std::string line;
    if (std::getline(file, line)) {
        // Line with headers is read and discarded
    }

    // start reading at second line to the end
    while (std::getline(file, line)) {
        categ_data data = parse_line(line);
        data_vector.push_back(data); // Add parsed data to the vector
    }

    file.close();

    // For demonstration, print the contents of the vector
    for (const auto& data : data_vector) {
        std::cout << "\ncat_id: " << data.cat_id << std::endl;
        std::cout << "\tgrp: " << data.grp << std::endl;
        std::cout << "\tshort_desc: " << data.short_desc << std::endl;
        std::cout << "\tincl_all: ";
        for (const auto& s : data.incl_all) std::cout << s << "|";
        std::cout << "\n";
        std::cout << "\tincl_any: ";
        for (const auto& s : data.incl_any) std::cout << s << "|";
        std::cout << "\n";
        std::cout << "\tincl_none: ";
        for (const auto& s : data.incl_none) std::cout << s << "|";
        std::cout << "\n";
        std::cout << "\tuom_incl_any: ";
    }

    return 0;
}
