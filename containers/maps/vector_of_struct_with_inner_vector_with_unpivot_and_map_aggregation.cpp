// Example Usage of Vector of Struct with and Embedded Vector
// Also a method using a map to perform aggregation using a map within a map since there are essentially two different Group By fields (one is area and the other is Categ)
// The inner map pait contains the categ string and then a double that used the += operator to aggregate in the loop within the aggregateData Function

#include<iostream>
#include<map>
#include<string>
#include<iomanip> // For setw
#include<vector>
#include<cmath>

struct cost_data {
    int row;
    std::string categ;
    double cost;
    std::vector<double> area;
    double areaSum;
};

struct unpivot_area_data {
    int area;
    int row;
    std::string categ;
    double area_cost;
    double areaQty;
};

void calculateAreaSum(std::vector<cost_data>& cd);
void print_vector_of_structure(std::vector<cost_data>& cd);
void calculateUnpivotAreaData(std::vector<cost_data>& cd, std::vector<unpivot_area_data>& unp);
void print_unpivot_data(std::vector<unpivot_area_data>& data);
void aggregateData(const std::vector<unpivot_area_data>& unp);
bool isZero(double chk_value);

int main() {

    std::vector<cost_data> cd;
    // 1. Load some example data
    cd.push_back({ 1, "pipe" , 1015.35, { 10.0, 20.0, 30.0 }} );
    cd.push_back({ 2, "pipe" , 2015.35, { 5.0,  20.0, 30.0 }} );
    cd.push_back({ 3, "pipe" , 2015.35, { 5.0,  20.0, 30.0 }} );
    cd.push_back({ 4, "steel", 6015.35, { 10.0, 20.0, 54.0 }} );
    cd.push_back({ 5, "equip", 735.45,  { 0.0,  0.0,  0.0  }} );       // area qtys are intentionally blank
    cd.push_back({ 6, "steel", 23.65,   { 5.0,  89.0, 7.0  }} );

    // 2. Calculate the Sum of Each Area 
    calculateAreaSum(cd);
    print_vector_of_structure(cd);

    // 3. Unpivot the Cost Data for each area placing any blank data in area 0
    std::vector<unpivot_area_data> unp;
    calculateUnpivotAreaData(cd, unp);
    print_unpivot_data(unp);

    // 4. Create a map to aggregate the cost data by area and category
    aggregateData(unp);

    return 0;
}

void calculateAreaSum(std::vector<cost_data>& cd) {
    for (auto& d : cd) {
        d.areaSum = 0;
        for (const auto& a : d.area) {
            d.areaSum += a;
        }
    }
}

void calculateUnpivotAreaData(std::vector<cost_data>& cd, std::vector<unpivot_area_data>& unp) {
    //Iterate through the cost data and generate the unpivotated data which will be pushed back to the 'unp' vector of struct <unpivot_area_data>
    for (auto& d : cd) {
        int areaItt=0;
        for (const auto& a : d.area) {
            double area_frac_cost = 0;
            if(!isZero(d.areaSum)) {
                area_frac_cost = d.cost * a / d.areaSum;
            } else {
                if(areaItt == 0) {
                    area_frac_cost = d.cost;    // If the area sum is equal to zero then assign all that cost to area 0
                }
            }
            unp.push_back( { areaItt, d.row, d.categ, area_frac_cost, a } );
            areaItt++;
        }
    }
}

void print_vector_of_structure(std::vector<cost_data>& data) {
    std::cout << "\nCost Data Entered:\n";
    std::cout << std::setw(5) << std::left << "row" << "|" << std::setw(10) << "categ" << "|" << std::setw(10) << "cost" << "|";

    // Print Area Headings
    const auto& area = data.front().area;
    for (size_t i = 0; i < area.size(); ++i) {
        std::cout << std::setw(10) <<  "Area" + std::to_string(i) << "|";
    }
    std::cout << std::setw(10) << "AreaSum";
    std::cout << "\n";

    for (const auto& d : data) {
        std::cout << std::setw(5) << std::left << d.row << "|" << std::setw(10) << d.categ << "|" << std::setw(10) << d.cost << "|";
        for (const auto& a : d.area) {
            std::cout << std::setw(10) << a << "|";
        }
        std::cout << d.areaSum << "\n";
    }
}

void print_unpivot_data(std::vector<unpivot_area_data>& data) {
    std::cout << "\nUnpivoted Data By Area:\n";
    std::cout << std::setw(5) << std::left << "area" << "|" << std::setw(5) << "row" << "|" << std::setw(10) << "categ" << "|" << std::setw(10) << "area_cost" << "|" << std::setw(10) << "area_qty" << "\n";

    for (const auto& d : data) {
        std::cout << std::setw(5) << std::left << d.area << "|" << std::setw(5) << d.row << "|" << std::setw(10) << d.categ << "|" << std::setw(10) << d.area_cost << "|" << std::setw(10) << d.areaQty << "\n";
    }
}

void aggregateData(const std::vector<unpivot_area_data>& unp) {
    // Create a nested map to hold the aggregated data
    std::map<int, std::map<std::string, double>> aggregation;

    for (const auto& d : unp) {
        aggregation[d.area][d.categ] += d.area_cost;
    }

    // Print the aggregated data
    std::cout << "\nAggregated Data:\n";
    std::cout << std::setw(5) << std::left << "area" << "|" << std::setw(10) << "categ" << "|" << std::setw(10) << "sum_cost" << "\n";

    for (const auto& area_pair : aggregation) {
        for (const auto& categ_pair : area_pair.second) {
            std::cout << std::setw(5) << std::left << area_pair.first << "|" << std::setw(10) << categ_pair.first << "|" << std::setw(10) << categ_pair.second << "\n";
        }
    }
}

bool isZero(double chk_value) {
    double epsilon = 1e-10;
    return std::fabs(chk_value) < epsilon;
}