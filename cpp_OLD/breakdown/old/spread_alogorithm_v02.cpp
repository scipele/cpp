#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

// Define structures to store spread items, basis, and estimate data
struct spread_basis {
    std::string cost_type;
    std::string div;
    std::string discipline;
    std::string lab_type;
};

struct estim_data {
    int row_id;
    std::string desc;
    double cost;
    spread_basis sb;     // structure of various helper fields that define cost types ( Labor, Material, Discipline, Company Division-Cost Center)
    std::vector<bool> apply_TF;
    std::vector<double> sprd_amt;
};

struct spread_costs_items {
    int spread_id;
    std::string spread_desc;
    double amount_to_spread;
    double spread_basis_aggregate;
    spread_basis sb;  // structure of various helper fields that define cost types ( Labor, Material, Discipline, Company Division-Cost Center)
};

// Function Prototypes
void get_sample_data(std::vector<estim_data>& ed, std::vector<spread_costs_items>& sprds);
bool is_str_match_w_wildcard(std::string& str1, std::string& str2);
void populate_spread_true_false_for_records(std::vector<estim_data>& ed, std::vector<spread_costs_items>& sprds);
void compute_spread_amounts(std::vector<estim_data>& ed, std::vector<spread_costs_items>& sprds);
void print_estim_data_w_spreads(std::vector<estim_data>& ed, std::vector<spread_costs_items>& sprds);

int main(int argc, char const *argv[])
{
    // 1. Define sample vectors
    std::vector<estim_data> ed;
    std::vector<spread_costs_items> sprds;

    // 2. Get the sample data and print to make sure that it was read properly
    get_sample_data(ed, sprds);

    // 3. Loop thru each estimate record and determine which spreads should be applied to the line items
    populate_spread_true_false_for_records(ed, sprds);

    // 4. Comput Spread Amounts / print recap of data and spreads
    compute_spread_amounts(ed, sprds);
    print_estim_data_w_spreads(ed, sprds);

    return 0;
}

void get_sample_data (std::vector<estim_data>& ed, std::vector<spread_costs_items>& sprds) {
    // Add example elements to the estimate data vector
    ed.push_back( {1, "pipe weld 3in s-80", 1563.63, { "L", "MechFld", "Pipe", "Dir" } });
    ed.push_back( {2, "pipe weld 4in s-80", 7658.78, { "L", "MechFld", "Pipe", "Dir" } });
    ed.push_back( {3, "Steel Erection"    , 8597.24, { "L", "MechFld", "Steel", "Dir"} });
    ed.push_back( {4, "15T Crane"         , 7000.63, { "L", "MechFld", "ConstrEq", ""} });
    ed.push_back( {5, "weld machine"      , 6454.75, { "L", "MechFld", "ConstrEq", ""} });

    // Add example elements to the spread cost items vector
    sprds.push_back( {1, "spread pipe lab productivity", 1500.35, 0.0, { "L", "MechFld", "Pipe", "Dir" } });
    sprds.push_back( {2, "tax on equip", 1347.53, 0.0, { "L", "MechFld", "ConstrEq", "" } });
    sprds.push_back( {3, "spread cop", 1500.35, 0.0, { "*", "*", "*", "*" } });
}

void populate_spread_true_false_for_records(std::vector<estim_data>& ed, std::vector<spread_costs_items>& sprds) {
    for (auto& e : ed) {
        e.apply_TF.resize(sprds.size(), false);
        e.sprd_amt.resize(sprds.size(), 0.0);

        for (std::size_t i = 0; i < sprds.size(); ++i) {
            auto& spr = sprds[i];
            if (is_str_match_w_wildcard(e.sb.cost_type, spr.sb.cost_type) &&
                is_str_match_w_wildcard(e.sb.div, spr.sb.div) &&
                is_str_match_w_wildcard(e.sb.discipline, spr.sb.discipline) &&
                is_str_match_w_wildcard(e.sb.lab_type, spr.sb.lab_type)) {
                e.apply_TF[i] = true;
            }
        }
    }
}

bool is_str_match_w_wildcard(std::string& str1, std::string& str2) {
    if (str1 == str2) {
        return true;
    }
    if (str2 == "*") {
        return true;
    }
    return false;
}

void compute_spread_amounts(std::vector<estim_data>& ed, std::vector<spread_costs_items>& sprds) {
    // i - iterates through the different spread categories
    for (int i=0; i<sprds.size(); i++) {
        
        // Loop through each record in the estimate data 'ed'
        for (int j = 0; j<ed.size(); j++) {
            if(ed[j].apply_TF[i]) {
                sprds[i].spread_basis_aggregate += ed[j].cost;
            }
        }
    }
    
    // Now calculate the spread amounts now that the aggregates are available from above
    // Loop through each spread type and calculate the spread amount for each record
    for (int i=0; i<sprds.size(); i++) {

        // Loop through each record in the estimate data 'ed'
        for (int j = 0; j<ed.size(); j++) {
            if(ed[j].apply_TF[i]) {
                if(sprds[i].spread_basis_aggregate > 0) {
                    ed[j].sprd_amt[i] = sprds[i].amount_to_spread * ed[j].cost / sprds[i].spread_basis_aggregate;
                } else {
                    ed[j].sprd_amt[i] = 0;
                }
            }
        }
    }
}

void print_estim_data_w_spreads(std::vector<estim_data>& ed, std::vector<spread_costs_items>& sprds) {
    
    std::cout << std::string(25, '\n');    // clear screen
    std::cout << std::endl;

    // print titles 
    std::cout   << "\n\n" << std::left
                << std::setw(12) << "row_id" 
                << std::setw(30) << "desc"
                << std::setw(12) << "cost "
                << std::setw(12) << "cost_type"
                << std::setw(12) << "div"
                << std::setw(12) << "discipline"
                << std::setw(12) << "lab_type";

    const int j_max = ed[0].apply_TF.size();
    // Print Headers for Double Nexted Loops Spread Match
    for (int j=0; j<j_max; j++) {
        std::cout << std::setw(8) << "Cond_" + std::to_string(j);
    }
    for (int j=0; j<j_max; j++) {
        std::cout << std::setw(10) << "Sprd_" + std::to_string(j);
    }
    std::cout << "\n";
    // print elements of the vector

    for (const auto& e : ed) {
        std::cout   << std::left
                    << std::setw(12) << e.row_id
                    << std::setw(30) << e.desc
                    << std::setw(12) << e.cost
                    << std::setw(12) << e.sb.cost_type
                    << std::setw(12) << e.sb.div
                    << std::setw(12) << e.sb.discipline
                    << std::setw(12) << e.sb.lab_type;

        for (int j=0; j<j_max; j++) {
            std::cout << std::setw(8) << std::boolalpha << e.apply_TF[j];
        }
        for (int j=0; j<j_max; j++) {
            std::cout << std::fixed << std::setprecision(2) << std::setw(10) << e.sprd_amt[j];
        }
        std::cout << "\n";
    }
}