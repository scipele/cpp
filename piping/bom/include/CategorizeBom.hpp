#pragma once
#include "Bom.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

class CategorizeBom {

public:
    // Define the struct
    struct categ_data {
        int cat_id;
        std::string grp;
        std::string short_desc;
        std::vector<std::string> incl_all;
        std::vector<std::string> incl_any;
        std::vector<std::string> incl_none;
    };
    
    // Enum to specify the type of check
    enum CheckType { ALL, ANY, NONE };

    // Public method to read and categorize data
    std::vector<categ_data> categ_vec;
    int Read_Categ() {
        categ_vec = read_file("c:/dev/cpp/piping/bom/cat_def.csv");
        // used for testing ->  print_data(categ_vec);
        return 0; // Success
    }

    int LookupCategories(std::vector<Bom>& bom) {
        // Categorize each description
        for (auto& bom_line : bom) {
            int cat_id = categorize(bom_line.desc, categ_vec);
            if (cat_id != -1) {
                bom_line.indx_code = cat_id;
                // testing only - std::cout << "Description: \"" << bom_line.desc << "\" matched category ID: " << cat_id << "\n";
            } else {
                // testing only - std::cout << "Description: \"" << bom_line.desc << "\" did not match any category.\n";
            }
        }
        LookupGrpAndShortDesc(bom); // Construct a map and fill in the group and short desc
        return 0; // Success
    }

private:
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

    std::vector<categ_data> read_file(const std::string& filename) {

        std::vector<categ_data> categ_vec;
        std::ifstream file(filename); // Open CSV file
        if (!file.is_open()) {
            std::cerr << "Error opening file!" << std::endl;
            return categ_vec;
        }

        std::string line;
        if (std::getline(file, line)) {
            // Line with headers is read and discarded
        }

        // start reading at second line to the end
        while (std::getline(file, line)) {
            categ_data data = parse_line(line);
            categ_vec.push_back(data); // Add parsed data to the vector
        }

        file.close();
        return categ_vec;
    }

    // Combined function to check substrings based on the CheckType
    bool contains_check(const std::string& str, const std::vector<std::string>& substrings, CheckType checkType) {
        auto predicate = [&](const std::string& s) {
            return str.find(s) != std::string::npos;
        };
        
        switch (checkType) {
            case ALL:
                return std::all_of(substrings.begin(), substrings.end(), predicate);
            case ANY:
                return std::any_of(substrings.begin(), substrings.end(), predicate);
            case NONE:
                return std::none_of(substrings.begin(), substrings.end(), predicate);
            default:
                return false; // In case of an invalid CheckType
        }
    }

    // Function to categorize a BOM description
    int categorize(const std::string& description, const std::vector<categ_data>& categories) {
        for (const auto& cat : categories) {
            if (contains_check(description, cat.incl_all, ALL) &&
                contains_check(description, cat.incl_any, ANY) &&
                contains_check(description, cat.incl_none, NONE)) {
                return cat.cat_id; // Return the matching category ID
            }
        }
        return -1; // No match found
    }

    void LookupGrpAndShortDesc(std::vector<Bom>& bom) {
        std::map<int, categ_data> lookup_cat_map;
        // Create the lookup map with cat_id as the key
        for (const auto& cat_item : categ_vec) {
            lookup_cat_map[cat_item.cat_id] = cat_item;     // Creates a map where we can lookup any item in the struct using the cat_id
        }

        // now loop thru the bom and fill in the data from the map
        for( auto& b : bom) {
            b.grp = lookup_cat_map[b.indx_code].grp;
            b.short_desc = lookup_cat_map[b.indx_code].short_desc;
        }
    }


    // Used only for testing purposes
    void print_data(const std::vector<categ_data>& categ_vec) {
        // print the contents of the vector to confirm read
        for (const auto& data : categ_vec) {
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
    }
};