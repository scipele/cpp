// Example Usage of a Map

#include<iostream>
#include<map>
#include<string>
#include<iomanip> // For setw
#include<vector>

struct cost_data {
    int row;
    std::string categ;
    double cost;
    std::vector<double> area;
    double areaSum;
};

void calculateAreaSum(std::vector<cost_data>& cd);
void print_vector_of_structure(std::vector<cost_data>& cd);

int main() {

    std::vector<cost_data> cd;
    // Load some example data
    cd.push_back({ 1, "pipe" , 1015.35, { 10.0, 20.0, 30.0 }} );
    cd.push_back({ 2, "pipe" , 2015.35, { 5.0,  20.0, 30.0 }} );
    cd.push_back({ 3, "steel", 6015.35, { 10.0, 20.0, 54.0 }} );
    cd.push_back({ 4, "equip", 735.45,  { 18.0, 37.0, 30.0 }} );
    cd.push_back({ 5, "steel", 23.65,   { 5.0,  89.0, 7.0  }} );

    calculateAreaSum(cd);
    print_vector_of_structure(cd);

    return 0;
}

void calculateAreaSum(std::vector<cost_data>& cd) {
    for (auto& d : cd) {
        for (const auto& a : d.area) {
            d.areaSum += a;
        }
    }
}

void print_vector_of_structure(std::vector<cost_data>& cd) {
    std::cout << "\nCost Data Entered:\n";
    std::cout << std::setw(5) << std::left << "row" << "|" << std::setw(10) << "categ" << "|" << std::setw(10) << "cost" << "|";

    // Print Area Headings
    const auto& area = cd.front().area;
    for (size_t i = 0; i < area.size(); ++i) {
        std::cout << std::setw(10) <<  "Area" + std::to_string(i) << "|";
    }
    std::cout << std::setw(10) << "AreaSum";
    std::cout << "\n";

    for (const auto& d : cd) {
        std::cout << std::setw(5) << std::left << d.row << "|" << std::setw(10) << d.categ << "|" << std::setw(10) << d.cost << "|";
        for (const auto& a : d.area) {
            std::cout << std::setw(10) << a << "|";
        }
        std::cout << d.areaSum << "\n";
    }
}