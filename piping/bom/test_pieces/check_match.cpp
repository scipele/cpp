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

// Helper function to check if a string contains all specified substrings
bool contains_all(const std::string& str, const std::vector<std::string>& substrings) {
    return std::all_of(substrings.begin(), substrings.end(), [&](const std::string& s) {
        return str.find(s) != std::string::npos;
    });
}

// Helper function to check if a string contains any of the specified substrings
bool contains_any(const std::string& str, const std::vector<std::string>& substrings) {
    return std::any_of(substrings.begin(), substrings.end(), [&](const std::string& s) {
        return str.find(s) != std::string::npos;
    });
}

// Helper function to check if a string contains none of the specified substrings
bool contains_none(const std::string& str, const std::vector<std::string>& substrings) {
    return std::none_of(substrings.begin(), substrings.end(), [&](const std::string& s) {
        return str.find(s) != std::string::npos;
    });
}

// Function to categorize a BOM description
int categorize(const std::string& description, const std::vector<categ_data>& categories) {
    for (const auto& cat : categories) {
        if (contains_all(description, cat.incl_all) &&
            contains_any(description, cat.incl_any) &&
            contains_none(description, cat.incl_none)) {
            return cat.cat_id; // Return the matching category ID
        }
    }
    return -1; // No match found
}

int main() {
    // Example category definitions
    std::vector<categ_data> categories = {
        {14, "FLG", "FLANGE-SW", {"SW", "FLANGE"}, {"SW FLANGE", "SW FLG", "RFSW"}, {"RED", "WN"}}
        // Add more categories as needed
    };

    // Example BOM descriptions
    std::vector<std::string> descriptions = {
        "150# RFSW FLANGE A182-F316/316L SS S/80s BORE",
        // Add more descriptions as needed
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
