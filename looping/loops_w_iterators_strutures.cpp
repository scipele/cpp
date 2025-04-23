#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

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

// function prototypes
void get_sample_data (std::vector<estim_data>& ed, std::vector<spread_costs_items>& sprds);
void loopingExample1 (std::vector<estim_data>& ed, std::vector<spread_costs_items>& sprds);
void loopingExample2 (std::vector<estim_data>& ed, std::vector<spread_costs_items>& sprds);
void loopingExample3 (std::vector<estim_data>& ed, std::vector<spread_costs_items>& sprds);

int main(int argc, char const *argv[])
{
    // Define sample vectors for the estimate data structure type and spread_costs_items type
    std::vector<estim_data> ed;
    std::vector<spread_costs_items> sprds;
    get_sample_data(ed, sprds);
    
    std::cout << std::string(30, '\n');    // clear screen
    std::cout << std::endl;

    loopingExample1(ed, sprds);
    loopingExample2(ed, sprds);
    loopingExample3(ed, sprds);

    return 0;
}


void get_sample_data (std::vector<estim_data>& ed, std::vector<spread_costs_items>& sprds) {
    // Add example elements to the estimate data vector
    ed.push_back( {1, "pipe weld 3in s-80", 1563.63, { "L", "MechFld", "Pipe", "Dir" }, { true, false, true}, { 254.38, 0, 75.01 } });
    ed.push_back( {2, "pipe weld 4in s-80", 7658.78, { "L", "MechFld", "Pipe", "Dir" }, { true, false, true}, { 1245.97, 0, 367.41} });
    ed.push_back( {3, "Steel Erection"    , 8597.24, { "L", "MechFld", "Steel", "Dir"}, { true, false, true}, { 0, 0, 412.43 } });
    ed.push_back( {4, "15T Crane"         , 7000.63, { "L", "MechFld", "ConstrEq", ""}, { true, false, true}, { 0, 701.1, 335.84 } });
    ed.push_back( {5, "weld machine"      , 6454.75, { "L", "MechFld", "ConstrEq", ""}, { true, false, true}, { 0, 646.43, 309.65 } });

    // Add example elements to the spread cost items vector
    sprds.push_back({1, "spread pipe lab productivity", 1500.35, 0.0, { "L", "MechFld", "Pipe", "Dir" } });
    sprds.push_back({2, "tax on equip", 1347.53, 0.0, { "L", "MechFld", "ConstrEq", "" } });
    sprds.push_back({3, "spread cop", 1500.35, 0.0, { "*", "*", "*", "*" } });
}


void loopingExample1 (std::vector<estim_data>& ed, std::vector<spread_costs_items>& sprds) {
    //1. Example method to iterate thru multi dimentioned data
    std::cout << "\n1. Method using range based for loop \n";    
    for (auto& e : ed) {
        std::cout   << std::setw(8) << std::left << e.row_id << " | " 
                    << std::setw(20) << e.desc << " | "
                    << std::setw(8) << e.sb.cost_type << " | "
                    << std::setw(10) << e.sb.div << " | "
                    << std::setw(10) << e.sb.discipline << " | "
                    << std::setw(8) << e.sb.lab_type << " | ";

        // Here is an example of how to iterate thru two differnt vectors together kinda in paralell (starting with first element of each vector)

        //  Initialize an iterator which is basically just a pointer to the memory address of the first element in the vector in this case
        //  given the struct definition std::vector<bool> apply_TF;
        //  short hand syntax used the auto keyword auto it1 = e.apply_TF.begin(); 

        std::vector<bool>::iterator it1 = e.apply_TF.begin(); 
        std::vector<double>::iterator it2 = e.sprd_amt.begin();

        while (it1 != e.apply_TF.end() && it2 != e.sprd_amt.end() ) {
            std::cout << std::setw(8) << std::boolalpha << *it1 << " | "
            << std::setw(10) << *it2 << " | ";
            it1++;
            it2++;
        }
        std::cout << "\n";
    }
}
void loopingExample2 (std::vector<estim_data>& ed, std::vector<spread_costs_items>& sprds) {
    // 2. Second method using all iterators
    std::cout << "\n2. Method using iterator class which are pointers to memory loction of item in vector or other structure\n";
    
    std::vector<estim_data>::iterator it_ed = ed.begin();
    // you can also use the shorthand 'auto' keyword, or const auto (if you are not changing anything)
    // auto knows that you are wanting to 
    // auto it_ed = ed.bgein();
    
    for (std::vector<estim_data>::iterator it_ed = ed.begin(); it_ed < ed.end(); it_ed++) {
        std::cout   << std::setw(8) << std::left << it_ed->row_id << " | " 
                    << std::setw(20) << it_ed->desc << " | "
                    << std::setw(8) << it_ed->sb.cost_type << " | "
                    << std::setw(10) << it_ed->sb.div << " | "
                    << std::setw(10) << it_ed->sb.discipline << " | "
                    << std::setw(8) << it_ed->sb.lab_type << " | ";

        // Here is an example of how to iterate thru two differnt vectors together kinda in paralell (starting with first element of each vector)
        //  Initialize an iterator which is basically just a pointer to the memory address of the first element in the vector in this case
        //  given the struct definition std::vector<bool> apply_TF;

        std::vector<bool>::iterator it_tf = it_ed->apply_TF.begin(); 
        std::vector<double>::iterator it_sp_amt = it_ed->sprd_amt.begin();

        while (it_tf != it_ed->apply_TF.end() && it_sp_amt != it_ed->sprd_amt.end() ) {
            std::cout << std::setw(8) << std::boolalpha << *it_tf << " | "
            << std::setw(10) << *it_sp_amt << " | ";
            it_tf++;
            it_sp_amt++;
        } 
        std::cout << "\n";
    }
}

void loopingExample3 (std::vector<estim_data>& ed, std::vector<spread_costs_items>& sprds) {
    // 3. Third method using all iterators and indivdual for loops
    std::cout << "\n3. Method using iterator class which are pointers to memory loction of item in vector or other structure\n";
    
    // you can also use the shorthand 'auto' keyword, or const auto (if you are not changing anything)
    // auto knows that you are wanting to 
    // auto it_ed = ed.bgein();
    
    auto it_ed = ed.begin();  //reset to the start since already initialied above
    
    for (it_ed; it_ed < ed.end(); it_ed++) {
        std::cout   << std::setw(8) << std::left << it_ed->row_id << " | " 
                    << std::setw(20) << it_ed->desc << " | "
                    << std::setw(8) << it_ed->sb.cost_type << " | "
                    << std::setw(10) << it_ed->sb.div << " | "
                    << std::setw(10) << it_ed->sb.discipline << " | "
                    << std::setw(8) << it_ed->sb.lab_type << " | ";

        // Here is an example of how to iterate thru two differnt vectors within the same structure using additional iterators in for loops
        for (auto it_tf = it_ed->apply_TF.begin(); it_tf < it_ed->apply_TF.end(); ++it_tf ) {
            std::cout << std::setw(8) << std::boolalpha << *it_tf << " | ";
        } 

        for (auto it_sp_amt = it_ed->sprd_amt.begin(); it_sp_amt < it_ed->sprd_amt.end(); ++it_sp_amt ) {
            std::cout << std::setw(8) << std::boolalpha << *it_sp_amt << " | ";
        } 

        std::cout << "\n";
    }
}
