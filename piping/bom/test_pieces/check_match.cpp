#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

// Define the struct for category data
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

int main() {
    // Example category definitions
    std::vector<categ_data> categories = {
        {1,  "B_G", "BOLTING"       , { }               , { "STUD" }                        , { }                },
        {14, "FLG", "FLANGE-SW"     , {"SW", "FLANGE"}  , {"SW FLANGE", "SW FLG", "RFSW"}   , {"RED", "WN"      }},
        {38, "MISC", "ROUND PLUG"   , { }               , { "ROUND PLUG", "PLUG"}           , {"VALVE", "VLV"   }},
        {39, "NIP", "NIPPLE"        , { }               , { "NIPPLE", "NIP"}                , {                 }
        
        }
        // Add more categories as needed
    };

    // Example BOM descriptions
    std::vector<std::string> descriptions = {
        "LONG A193-B7 STUDS W/2 HH A194-2H NUTS",
        "150# RFSW FLANGE A182-F316/316L SS S/80s BORE",
        "THRD ROUND PLUG A182-F316SS",
        "LONG NIPPLE S/160 BORE A106B SMLS CS (POExTOE)"
    };

    // Categorize each description
    for (const auto& desc : descriptions) {
        int cat_id = categorize(desc, categories);
        if (cat_id != -1) {
            std::cout << "Description: \"" << desc << "\" matched category ID: " << cat_id << "\n";
        } else {
            std::cout << "Description: \"" << desc << "\" did not match any category.\n";
        }
    }

    return 0;
}
