#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <locale>
#include <cmath>  // For std::round
#include <map>

struct estim_data {
    long rowNo;
    std::string filterX;
    std::string desc;
    std::string brkd_ref;
    double other_mh;
    std::string param1;
    std::string param2;
    std::string param3;
    std::string type;
    double qty;
    std::string uom;
    double umh;
    double mh_tot;
    double rate;
    double labor;
    double matl;
    double sub;
    double eq;
    double total;
    std::string div;
    std::string discp;
    std::string labtype;
    std::vector<double> area_qty;
    double areaSum;
};

struct unpivot_area_data {
    int areaNo;
    long rowNo;
    std::string brkd_ref;
    double other_mh;
    double area_qty;
    double mh_tot;
    double labor;
    double matl;
    double sub;
    double eq;
    double total;
};

struct AggregatedData {
    double other_mh = 0;
    double mh_tot = 0;
    double area_qty = 0;
    double labor = 0;
    double matl = 0;
    double sub = 0;
    double eq = 0;
    double total = 0;
};

//Function Prototypes
int openBinaryFile(std::vector<estim_data>& orig_data);
void readStructure(std::ifstream &file, estim_data &s);
void readString(std::ifstream &file, std::string &str);
void printToScreen(std::vector<estim_data>& orig_data);
int filterOutDataRemoveZerosAndTotals(std::vector<estim_data>& orig_data, std::vector<estim_data>& cleaned_data, std::vector<estim_data>& totals_check);
std::string formatCurrency(double value);
std::string formatFixed(double value);
void checkTotals(std::vector<estim_data>& cleaned_data, std::vector<unpivot_area_data>& unpiv, std::vector<estim_data>& totals_check);
double roundToTwoDecimalPlaces(double value);
void deleteUnusedAreaData(std::vector<estim_data>& data);
void calculateUnpivotAreaData(std::vector<estim_data>& cleaned_data, std::vector<unpivot_area_data>& unpiv);
void print_unpivot_data(std::vector<unpivot_area_data>& unpiv);
bool isZero(double chk_value);
void aggregateData(const std::vector<unpivot_area_data>& unpiv, std::map<int, std::map<std::string, AggregatedData>>& aggregationMap);

int main() {

    // 1. Read in original data from the binary and print to screen for reference    
    std::vector<estim_data> orig_data;
    int result;
    result = openBinaryFile(orig_data);
    
    // 2. Now cleanup the data by removing zero lines, and also brkd_ref item = "-"
    std::vector<estim_data> totals_check;  //Store the original totals from binary read, calculated totals from ckeaned_lines, and deltas
    std::vector<estim_data> cleaned_data;
    result = filterOutDataRemoveZerosAndTotals(orig_data, cleaned_data, totals_check);
    deleteUnusedAreaData(cleaned_data);
    printToScreen(cleaned_data);
    
    // 3. Unpivot the Cost Data for each area placing any blank data in area 0
    std::vector<unpivot_area_data> unpiv;
    calculateUnpivotAreaData(cleaned_data, unpiv);
    print_unpivot_data(unpiv);

    // 4. Check the totals match the totals within the estimte sheet calculation
    checkTotals(cleaned_data, unpiv, totals_check);

    // 5. Create a map to aggregate the unpivoted hours and cost data with Group by area and category
     std::map<int, std::map<std::string, AggregatedData>> aggregationMap;
    aggregateData(unpiv, aggregationMap);

    return 0;
}

int openBinaryFile(std::vector<estim_data>& orig_data) {
    std::ifstream file("C:/t/data.bin", std::ios::binary);
    if (!file) {
        std::cerr << "Unable to open file";
        return 1;
    }
    while (file.peek() != EOF) {
        estim_data ed;
        readStructure(file, ed);
        orig_data.push_back(ed);
    }
    file.close();
    return 0;
}
void readStructure(std::ifstream &file, estim_data &s) {
    int areaSize;
    file.read(reinterpret_cast<char*>(&s.rowNo), sizeof(s.rowNo));
    readString(file, s.filterX);
    readString(file, s.desc);
    readString(file, s.brkd_ref);
    file.read(reinterpret_cast<char*>(&s.other_mh), sizeof(s.other_mh));
    readString(file, s.param1);
    readString(file, s.param2);
    readString(file, s.param3);
    readString(file, s.type);
    file.read(reinterpret_cast<char*>(&s.qty), sizeof(s.qty));
    readString(file, s.uom);
    file.read(reinterpret_cast<char*>(&s.umh), sizeof(s.umh));
    file.read(reinterpret_cast<char*>(&s.mh_tot), sizeof(s.mh_tot));
    file.read(reinterpret_cast<char*>(&s.rate), sizeof(s.rate));
    file.read(reinterpret_cast<char*>(&s.labor), sizeof(s.labor));
    file.read(reinterpret_cast<char*>(&s.matl), sizeof(s.matl));
    file.read(reinterpret_cast<char*>(&s.sub), sizeof(s.sub));
    file.read(reinterpret_cast<char*>(&s.eq), sizeof(s.eq));
    file.read(reinterpret_cast<char*>(&s.total), sizeof(s.total));
    readString(file, s.div);
    readString(file, s.discp);
    readString(file, s.labtype);

    file.read(reinterpret_cast<char*>(&areaSize), sizeof(areaSize));
    s.area_qty.resize(areaSize);
    file.read(reinterpret_cast<char*>(s.area_qty.data()), areaSize * sizeof(double));
}
void readString(std::ifstream &file, std::string &str) {
      // Read length as a single byte, since written as a byte 0-255 numeric in vba
    unsigned char length;
    file.read(reinterpret_cast<char*>(&length), sizeof(length));
    str.resize(length);
    file.read(&str[0], length);
}
void printToScreen(std::vector<estim_data>& data) {
    // Print Titles    
    std::cout << std::setw(5) <<  std::left << "rowNo" << "|"
    << std::setw(3) << "X" << "|" 
    << std::setw(42) << "desc" << "|" 
    << std::setw(32) << "brkd_ref" << "|" 
    << std::setw(9) << "other_mh" << "|" 
    << std::setw(7) << "param1" << "|" 
    << std::setw(7) << "param2" << "|" 
    << std::setw(10) << "param3" << "|" 
    << std::setw(5) << "type" << "|" 
    << std::setw(8) << "qty" << "|" 
    << std::setw(10) << "uom" << "|" 
    << std::setw(10) << "umh" << "|" 
    << std::setw(10) << "mh_tot" << "|" 
    << std::setw(10) << "rate" << "|" 
    << std::setw(12) << "labor" << "|" 
    << std::setw(12) << "matl" << "|" 
    << std::setw(12) << "sub" << "|" 
    << std::setw(12) << "eq" << "|" 
    << std::setw(12) << "total" << "|" 
    << std::setw(8) << "div" << "|" 
    << std::setw(8) << "discp" << "|" 
    << std::setw(9) << "labtype" << "|";
    int i=0;
    for (const auto& s : data) {
        for (const auto &val : s.area_qty) {
            i++;
            std::cout << std::setw(10) << "Area " + std::to_string(i) << "|" ;
        }
        std::cout << "\n";
        break;
    }
    
    // Output to screen verify the read of the data
    for (const auto& s : data) {
        std::cout << std::setw(5) << std::left << s.rowNo << "|" 
            << std::setw(3) << s.filterX << "|" 
            << std::setw(42) << s.desc << "|" 
            << std::setw(32) << s.brkd_ref << "|" 
            << std::setw(9) << s.other_mh << "|" 
            << std::setw(7) << s.param1 << "|" 
            << std::setw(7) << s.param2 << "|" 
            << std::setw(10) << s.param3 << "|" 
            << std::setw(5) << s.type << "|" 
            << std::setw(8) << s.qty << "|" 
            << std::setw(10) << s.uom << "|" 
            << std::setw(10) << formatFixed(s.umh) << "|" 
            << std::setw(10) << formatFixed(s.mh_tot) << "|" 
            << std::setw(10) << formatFixed(s.rate) << "|" 
            << std::setw(12) << formatFixed(s.labor) << "|" 
            << std::setw(12) << formatFixed(s.matl) << "|" 
            << std::setw(12) << formatFixed(s.sub) << "|" 
            << std::setw(12) << formatFixed(s.eq) << "|" 
            << std::setw(12) << formatFixed(s.total) << "|" 
            << std::setw(8) << s.div << "|" 
            << std::setw(8) << s.discp << "|" 
            << std::setw(9) << s.labtype << "|" ;
        for (const auto &val : s.area_qty) {
            std::cout << std::setw(10) << val << "|" ;
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}
int filterOutDataRemoveZerosAndTotals(std::vector<estim_data>& orig_data, std::vector<estim_data>& cleaned_data, std::vector<estim_data>& totals_check) {
    // Copy elements with the sum of labor, matl, sub, and eq not equal to zero
    for (const auto& ed : orig_data) {
        if ((ed.labor + ed.matl + ed.sub + ed.eq) != 0  && ed.brkd_ref != "-" ) {
            cleaned_data.push_back(ed);
        }
    }
    // Save the last element totals using the .back() element from the original data 
    totals_check.push_back(orig_data.back());

    // delete original data
    orig_data.clear();

    return 0;
}
std::string formatCurrency(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << "$" << value;
    return oss.str();
}
std::string formatFixed(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    return oss.str();
}
double roundToTwoDecimalPlaces(double value) {
    return std::round(value * 100.0) / 100.0;
}
void deleteUnusedAreaData(std::vector<estim_data>& data) {
    std::size_t num_areas = data[0].area_qty.size();
    
    // Iterate from the end to the beginning of the 
    for (std::size_t i = num_areas; i > 0; --i) {
        double sum = 0.0;
        
        // Calculate the sum for the current area_qty dimension
        for (const auto& item : data) {
            sum += item.area_qty[i - 1];
        }
        
        // If the sum is 0, remove the current area dimension from all estim_data items
        if (sum == 0.0) {
            for (auto& item : data) {
                item.area_qty.erase(item.area_qty.begin() + i - 1);
            }
        }
    }
}
bool isZero(double chk_value) {
    double epsilon = 1e-10;
    return std::fabs(chk_value) < epsilon;
}

void calculateUnpivotAreaData(std::vector<estim_data>& cleaned_data, std::vector<unpivot_area_data>& unpiv) {
    
    //initialize some temp variables to keep running total of by area for checking purposes    
    double sum_other_mh = 0;
    double sum_mh_tot = 0;
    double sum_labor = 0;
    double sum_matl = 0;
    double sum_sub = 0;
    double sum_eq = 0;
    double sum_total = 0;

    //Iterate through the cost data and generate the unpivotated data which will be pushed back to the 'unp' vector of struct <unpivot_area_data>
    for (auto& d1 : cleaned_data) {
        for (const auto& atmp : d1.area_qty) {
            d1.areaSum += atmp;
        }
    }

    //Iterate through the cost data and generate the unpivotated data which will be pushed back to the 'unp' vector of struct <unpivot_area_data>
    for (auto& d : cleaned_data) {
        int areaItt=0;

        for (const auto& a : d.area_qty) {
            //set all temp fractional calcs equal to zero before each iteration
            double area_frac = 0;  double area_qty = 0; double frac_other_mh = 0; double frac_mh_tot = 0; double frac_labor = 0; double frac_matl = 0; double frac_sub = 0; double frac_eq = 0; double frac_total=0;

            if(!isZero(d.areaSum)) {    //Use a function to see if double is close to zero
                area_frac = a / d.areaSum;
                frac_other_mh = d.other_mh * area_frac;
                frac_mh_tot = d.mh_tot * area_frac;
                frac_labor = d.labor * area_frac;
                frac_matl = d.matl * area_frac;
                frac_sub = d.sub * area_frac;
                frac_eq = d.eq * area_frac;
            } else {
                if(areaItt == 0) {
                    frac_other_mh = d.other_mh;  // If the area sum is equal to zero then assign all that cost to areaItt = 0
                    frac_mh_tot = d.mh_tot;
                    frac_labor = d.labor;
                    frac_matl = d.matl;
                    frac_sub = d.sub;
                    frac_eq = d.eq;
                }
            }
            frac_total = frac_labor + frac_matl + frac_sub + frac_eq;

            unpiv.push_back( { areaItt, d.rowNo, d.brkd_ref, frac_other_mh, a, frac_mh_tot, frac_labor, frac_matl, frac_sub, frac_eq, frac_total } );
            // Compute Sum Total Checks
            sum_other_mh += frac_other_mh;
            sum_mh_tot += frac_mh_tot;
            sum_labor += frac_labor;
            sum_matl += frac_matl;
            sum_sub += frac_sub;
            sum_eq += frac_eq;
            sum_total += frac_total;
            areaItt++;
        }
    }
    // push Totals for Checking Purposes
    unpiv.push_back( { 0, 0, "Totals", sum_other_mh, 0, sum_mh_tot, sum_labor, sum_matl, sum_sub, sum_eq, sum_total } );
}


void print_unpivot_data(std::vector<unpivot_area_data>& unpiv) {
    std::cout << "\nUnpivoted Data By Area:\n";
    std::cout   << std::setw( 6) << std::left << "areaNo" << "|" 
                << std::setw( 6) << "rowNo" << "|"
                << std::setw(42) << "brkd_ref" << "|"
                << std::setw(10) << "other_mh" << "|"
                << std::setw(10) << "area_qty" << "|"
                << std::setw(10) << "mh_tot" << "|"
                << std::setw(10) << "labor" << "|"
                << std::setw(10) << "matl" << "|"
                << std::setw(10) << "sub" << "|"
                << std::setw(10) << "eq" << "|"
                << std::setw(10) << "total" << "\n";

    for (const auto& unp : unpiv) {
        std::cout   << std::setw( 6) << std::left << unp.areaNo << "|"
                    << std::setw( 6) << unp.rowNo << "|"
                    << std::setw(42) << unp.brkd_ref << "|"
                    << std::setw(10) << unp.other_mh << "|"
                    << std::setw(10) << unp.area_qty << "|"
                    << std::setw(10) << unp.mh_tot << "|"
                    << std::setw(10) << unp.labor << "|"
                    << std::setw(10) << unp.matl << "|"
                    << std::setw(10) << unp.sub << "|"
                    << std::setw(10) << unp.eq << "|"
                    << std::setw(10) << unp.total << "\n";
    }
}

void checkTotals(std::vector<estim_data>& cleaned_data, std::vector<unpivot_area_data>& unpiv, std::vector<estim_data>& totals_check) {
    // create a placeholder for the totals that will be reset to zero and recalculated
    totals_check.push_back(totals_check.back());

    totals_check[1].other_mh = 0.0;
    totals_check[1].mh_tot = 0.0;
    totals_check[1].labor = 0.0;
    totals_check[1].matl = 0.0;
    totals_check[1].sub = 0.0;
    totals_check[1].eq = 0.0;

    // create a placeholder for the later total from unpivot data and deltas
    totals_check.push_back(totals_check.back());
    totals_check.push_back(totals_check.back());
    totals_check.push_back(totals_check.back());

    // Add up totals for checking of data
    for (const auto& ed : cleaned_data) {
        totals_check[1].other_mh += ed.other_mh;
        totals_check[1].mh_tot += ed.mh_tot;
        totals_check[1].labor += ed.labor;
        totals_check[1].matl += ed.matl;
        totals_check[1].sub += ed.sub;
        totals_check[1].eq += ed.eq;
    }
    totals_check[1].total = totals_check[1].labor + totals_check[1].matl + totals_check[1].sub + totals_check[1].eq;

    //Get totals from Unpivot Data
    totals_check[2].other_mh = unpiv.back().other_mh;
    totals_check[2].mh_tot = unpiv.back().mh_tot;
    totals_check[2].labor = unpiv.back().labor;
    totals_check[2].matl = unpiv.back().matl;
    totals_check[2].sub = unpiv.back().sub;
    totals_check[2].eq = unpiv.back().eq;

    // calculate the deltas in totals
    for(int i=1; i<3; i++) {
    totals_check[i+2].other_mh = roundToTwoDecimalPlaces(totals_check[0].other_mh) - roundToTwoDecimalPlaces(totals_check[i].other_mh);
    totals_check[i+2].mh_tot = roundToTwoDecimalPlaces(totals_check[0].mh_tot) - roundToTwoDecimalPlaces(totals_check[i].mh_tot);
    totals_check[i+2].labor = roundToTwoDecimalPlaces(totals_check[0].labor) - roundToTwoDecimalPlaces(totals_check[i].labor);
    totals_check[i+2].matl = roundToTwoDecimalPlaces(totals_check[0].matl) - roundToTwoDecimalPlaces(totals_check[i].matl);
    totals_check[i+2].sub = roundToTwoDecimalPlaces(totals_check[0].sub) - roundToTwoDecimalPlaces(totals_check[i].sub);
    totals_check[i+2].eq = roundToTwoDecimalPlaces(totals_check[0].eq) - roundToTwoDecimalPlaces(totals_check[i].eq);
    totals_check[i+2].total = roundToTwoDecimalPlaces(totals_check[0].total) - roundToTwoDecimalPlaces(totals_check[i].total);
    }

  // Print titles
    std::cout << "\nThe following lines are a comparison of all the key Totals from the Estimate Sheet, and Checked Addition of the Total Hrs and Costs:\n";
    std::cout << std::setw(40) << std::left << "Item" 
              << std::setw(15) << std::left << "other_mh" 
              << std::setw(15) << std::left << "mh_tot" 
              << std::setw(15) << std::left << "Labor" 
              << std::setw(15) << std::left << "Matl" 
              << std::setw(15) << std::left << "Sub" 
              << std::setw(15) << std::left << "Equip" 
              << std::setw(15) << std::left << "Total" << "\n";    

    // Print values formatted as currency
    for(int i=0; i<5; i++) {
        
        std::string itemDesc[] = {"Estim Totals(0)", "Cleaned Data Totals(1)", "Unpivot Totals(2)", "Delta Totals(0-1)", "Delta Totals(0-2)"};
        
        std::cout << std::fixed << std::setprecision(2) << std::left;
        std::cout << std::setw(40) << itemDesc[i]
                << std::setw(15) << formatFixed(totals_check[i].other_mh)
                << std::setw(15) << formatFixed(totals_check[i].mh_tot)
                << std::setw(15) << formatCurrency(totals_check[i].labor)
                << std::setw(15) << formatCurrency(totals_check[i].matl)
                << std::setw(15) << formatCurrency(totals_check[i].sub)
                << std::setw(15) << formatCurrency(totals_check[i].eq)
                << std::setw(15) << formatCurrency(totals_check[i].total)
                << "\n";
    }
}

void aggregateData(const std::vector<unpivot_area_data>& unpiv, std::map<int, std::map<std::string, AggregatedData>>& aggregationMap) {

    for (const auto& unp : unpiv) {
        aggregationMap[unp.areaNo][unp.brkd_ref].other_mh += unp.other_mh;
        aggregationMap[unp.areaNo][unp.brkd_ref].mh_tot += unp.mh_tot;
        aggregationMap[unp.areaNo][unp.brkd_ref].area_qty += unp.area_qty;
        aggregationMap[unp.areaNo][unp.brkd_ref].labor += unp.labor;
        aggregationMap[unp.areaNo][unp.brkd_ref].matl += unp.matl;
        aggregationMap[unp.areaNo][unp.brkd_ref].sub += unp.sub;
        aggregationMap[unp.areaNo][unp.brkd_ref].eq += unp.eq;
    }

    // Print the aggregated data
    std::cout << "\nAggregated Data:\n";
    std::cout   << std::setw(5) << std::left << "area" << "|" 
                << std::setw(40) << "categ" << "|" 
              << std::setw(15) << std::left << "other_mh" << "|"
              << std::setw(15) << std::left << "mh_tot" << "|"
              << std::setw(15) << std::left << "area_qty" << "|"
              << std::setw(15) << std::left << "Labor" << "|"
              << std::setw(15) << std::left << "Matl" << "|"
              << std::setw(15) << std::left << "Sub" << "|"
              << std::setw(15) << std::left << "Equip" << "|"
              << std::setw(15) << std::left << "Total" << "\n";    

    for (auto& area_pair : aggregationMap) {
        for (auto& categ_pair : area_pair.second) {
            auto& data = categ_pair.second;
            
            data.total = data.labor + data.matl + data.sub + data.eq;

            std::cout   << std::setw(5) << std::left << area_pair.first << "|"
                        << std::setw(40) << categ_pair.first << "|"
                        << std::setw(15) << data.other_mh << "|"
                        << std::setw(15) << data.mh_tot << "|"
                        << std::setw(15) << data.area_qty << "|"
                        << std::setw(15) << data.labor << "|"
                        << std::setw(15) << data.matl << "|"
                        << std::setw(15) << data.sub << "|"
                        << std::setw(15) << data.eq << "|"
                        << std::setw(15) << data.total << "\n";
        }
    }
}