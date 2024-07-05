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
    std::string atl_btl;    // Above the Line (atl) or Below the Line (btl)
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
    ed.push_back( {1, "pipe weld 3in s-80", 1563.63, { "L", "MechFld", "Pipe"   , "Dir", "a" } });
    ed.push_back( {2, "pipe weld 4in s-80", 7658.78, { "L", "MechFld", "Pipe"   , "Dir", "a" } });
    ed.push_back( {3, "Steel Erection"    , 8597.24, { "L", "MechFld", "Steel"  , "Dir", "a" } });
    ed.push_back( {4, "15T Crane"         , 7000.63, { "L", "MechFld", "ConstrEq", ""  , "a" } });
    ed.push_back( {5, "weld machine"      , 6454.75, { "L", "MechFld", "ConstrEq", ""  , "a" } });
    ed.push_back( {6, "insul labor"       , 8543.25, { "L", "Spi"    , "Insul"   , ""  , "b" } });

    // Add example elements to the spread cost items vector
    sprds.push_back( {1, "spread pipe lab productivity", 1500.35, 0.0, { "L", "MechFld", "Pipe"    , "Dir", "a"} });
    sprds.push_back( {2, "tax on equip"                , 1347.53, 0.0, { "L", "MechFld", "ConstrEq", ""   , "a" } });
    sprds.push_back( {3, "spread cop"                  , 3565.0, 0.0 , { "*", "*"      , "*"       , "*"  , "a" } });
}

void populate_spread_true_false_for_records(std::vector<estim_data>& ed, std::vector<spread_costs_items>& sprds) {

    // iterator thru each record of the estimate data
    for (auto& e : ed) {
        
        // resize the ectors to match the number of spreads in the sprds vector
        e.apply_TF.resize(sprds.size(), false);
        e.sprd_amt.resize(sprds.size(), 0.0);

        // create iterator for std::vector<bool> apply_TF that is inside the estimate data vector 'ed' 
        auto it_tf = e.apply_TF.begin();
        // create iterator for the 'sprds' vector which contains 'sb' nested vector with fields cost_type, div, discipline, lab_type
        auto it_spr = sprds.begin();
        
        for (it_tf; it_tf < e.apply_TF.end(); ++it_tf) {
            if (is_str_match_w_wildcard(e.sb.cost_type, it_spr->sb.cost_type) &&
                is_str_match_w_wildcard(e.sb.div, it_spr->sb.div) &&
                is_str_match_w_wildcard(e.sb.discipline, it_spr->sb.discipline) &&
                is_str_match_w_wildcard(e.sb.lab_type, it_spr->sb.lab_type) &&
                is_str_match_w_wildcard(e.sb.atl_btl, it_spr->sb.atl_btl)) {
                *it_tf = true;
            }
            it_spr++;
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

    std::cout << std::string(25, '\n');    // clear screen
    std::cout << std::endl;

    // Loop through each record in the estimate data 'ed' using an iterator 
    auto it_ed = ed.begin();
    for (it_ed; it_ed < ed.end(); ++it_ed) {
   
        // Reset the iterators for each new records to begining since they both have the same number of items
        auto it_tf = it_ed->apply_TF.begin();
        auto it_spr = sprds.begin();
        
        while (it_tf != ed.begin()->apply_TF.end() && it_spr != sprds.end()) {

            if(*it_tf == true) {
                it_spr->spread_basis_aggregate += it_ed->cost;
            }
            it_tf++;
            it_spr++;
        }
    }

    for (auto sprd : sprds) {
        std::cout << "spread aggr " << std::fixed << std::setprecision(2) << sprd.spread_basis_aggregate << "\n";
    }
    
    // Now calculate the spread amounts for each record now aggregates are available from above
    // Loop through each record in the estimate data 'ed' using an iterator 
    it_ed = ed.begin();  // reset iterator from above to begin location
    for (it_ed; it_ed < ed.end(); ++it_ed) {    
    
        // Reset the iterators for each new records to begining since they both have the same number of items

        auto it_tf = it_ed->apply_TF.begin();
        auto it_spr = sprds.begin();
        auto it_spr_amt = it_ed->sprd_amt.begin();

        while (it_tf != ed.begin()->apply_TF.end() && it_spr != sprds.end() &&  it_spr_amt != it_ed->sprd_amt.end()) {
            
            if(*it_tf == true) {
                if(it_spr->spread_basis_aggregate > 0) {
                    *it_spr_amt = it_spr->amount_to_spread * it_ed->cost / it_spr->spread_basis_aggregate;
                } else {
                    *it_spr_amt = 0;
                }
            }
            it_tf++;
            it_spr++;
            it_spr_amt++;
        }
    }
}

void print_estim_data_w_spreads(std::vector<estim_data>& ed, std::vector<spread_costs_items>& sprds) {
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

    int i=0;    
    for (const auto sprd : sprds) {
        std::cout << std::setw(8) << "Cond_" + std::to_string(i);
        i++;
    }

    i=0;
    for (const auto sprd : sprds) {
        std::cout << std::setw(10) << "Sprd_" + std::to_string(i);
        i++;
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

        for (auto it_tf = e.apply_TF.begin(); it_tf < e.apply_TF.end(); it_tf++) {
            std::cout << std::setw(8) << std::boolalpha << *it_tf;
        }

        for (auto it_spr_amt = e.sprd_amt.begin(); it_spr_amt < e.sprd_amt.end(); it_spr_amt++ ) {
            std::cout << std::fixed << std::setprecision(2) << std::setw(10) << *it_spr_amt;
        }
        std::cout << "\n";
    }
}
