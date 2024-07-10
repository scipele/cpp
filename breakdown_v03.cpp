#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <locale>
#include <cmath>  // For std::round
#include <map>

// Define error codes
enum ErrorCode {
    SUCCESS = 0,
    ERROR_BLANK_ESTIM_DATA = -1,
};

struct gen_info {
    unsigned short row_cnt_ed;         // Read as a 2 byte short to coorespond with vba integer type
    unsigned short row_cnt_sd;      // Read as a 2 byte short to coorespond with vba integer type
    unsigned short area_cnt;           // Read as a 2 byte short to coorespond with vba integer type
};

struct spread_items {
    unsigned short sprd_order;           // Read as a 2 byte short to coorespond with vba integer type
    std::string sprd_desc;
    std::string type;
    std::string div;
    std::string discp;
    std::string lab_type;
    std::string atl_btl;
    std::string sprd_basis_field;
    std::string incl_cumul_sprd_values_list;
    double amt_to_spr_l;
    double amt_to_spr_m;
    double amt_to_spr_s;
    double amt_to_spr_e;
    double spr_l_agr;
    double spr_m_agr;
    double spr_s_agr;
    double spr_e_agr;
    };

struct estim_data {
    long rowNo;
    std::string filterX;
    std::string desc;
    std::string brkd_ref;
    double other_mh;
    std::string param1;
    std::string param2;
    std::string param3;
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
    std::string type;
    std::string div;
    std::string discp;
    std::string lab_type;
    std::string atl_btl = "a";    // Plaeholder for above the Line (atl) or Below the Line (btl)
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
    std::string type;
    std::string div;
    std::string discp;
    std::string lab_type;
    std::string atl_btl = "a";    // Plaeholder for above the Line (atl) or Below the Line (btl)
    std::vector<bool> apply_TF;
    std::vector<double> sprd_lab;
    std::vector<double> sprd_mat;
    std::vector<double> sprd_sub;
    std::vector<double> sprd_eqp;
};


struct aggregated_data {
    int areaNo;
    std::string brkd_ref;
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
/*1.*/ int readDataFromBinaryFile(std::vector<estim_data>& orig_data, std::vector<spread_items>& sd, gen_info& gi);
    /*1.1*/ void readStructure1(std::ifstream& file, estim_data& s, gen_info& gen_info);
    /*1.2*/ void readStructure2(std::ifstream& file, spread_items& temp_sd);
    /*1.3*/ void readString(std::ifstream &file, std::string &str);

// 2. Now cleanup the data by removing zero lines, and also brkd_ref item = "-"
    /*2.1*/ int filterOutDataRemoveZerosAndTotals(std::vector<estim_data>& orig_data, std::vector<estim_data>& cleaned_data, std::vector<estim_data>& totals_check);
    /*2.2*/ int locateAndTagBelowLineItems(std::vector<estim_data>& cleaned_data);
    /*2.3*/ void deleteUnusedAreaData(std::vector<estim_data>& data);
    /*2.4*/ void print_ed(std::vector<estim_data>& orig_data);

// 3. Unpivot the Cost Data for each area placing any blank data in area 0
    /*3.1*/ void calculateUnpivotAreaData(std::vector<estim_data>& cleaned_data, std::vector<unpivot_area_data>& upv);

/*4.*/  void checkTotals(std::vector<estim_data>& cleaned_data, std::vector<unpivot_area_data>& upv, std::vector<estim_data>& totals_check);

// 5. Loop thru each estimate record and determine which spreads should be applied to the line items
    /*5.1*/ void populate_spread_true_false_for_records(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd);
    /*5.2*/ bool is_str_match_w_wildcard(std::string& str1, std::string& str2);
    /*5.3*/ void compute_amt_to_spread_aggregates(const std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd );
    /*5.4*/ void remove_spread_lines_from_unpivot_data(std::vector<unpivot_area_data>& upv);
    /*5.5*/ void compute_spread_basis_aggregates(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd);

    /*5.6*/ void compute_spread_amounts(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd);
    /*5.7*/ void check_spread_amounts_totals(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd);
    /*5.8*/ void compute_sum_base_and_spread_amounts(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd, std::vector<aggregated_data>& fd);

//6. Print Results to Screen
    /*6.1*/ void print_unpivot_data(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd);
    /*6.2*/ void print_sd(std::vector<spread_items>& sd);
    /*6.3*/ void print_final_data(std::vector<aggregated_data>& fd);

//7. Write Binary Output File for Excel Import
    /*7.1*/ void open_file_to_write_results_binary(std::ofstream& outFile);
    /*7.2*/ void write_unpivot_data(std::ofstream& outFile, std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd);
    /*7.3*/ void write_sd(std::ofstream& outFile, std::vector<spread_items>& sd);
    /*7.4*/ void write_final_data(std::ofstream& outFile, std::vector<aggregated_data>& fd);

//20. Misc Functions
    /*20.1*/ std::string formatCurrency(double value);
    /*20.2*/ std::string formatFixed(double value);
    /*20.3*/ double roundToTwoDecimalPlaces(double value);
    /*20.4*/ bool isZero(double chk_value);
    /*20.5*/ void printAnyErrors(int result, int estim_row);

// **************************************************
// ******************  MAIN  ************************
// **************************************************

int main(int argc, char const *argv[]) {

    // 0. Declare key structs
    std::vector<estim_data> orig_data;
    std::vector<spread_items> sd;
    gen_info gi;

    // 1. Read in original data from the binary and print to screen for reference    
    readDataFromBinaryFile(orig_data, sd, gi);
    //print_ed(orig_data);

    std::vector<estim_data> totals_check;  //Store the original totals from binary read, calculated totals from ckeaned_lines, and deltas
    std::vector<estim_data> cleaned_data;

    // 2. Now cleanup the data by removing zero lines, and also brkd_ref item = "-"
        // 2.1
        filterOutDataRemoveZerosAndTotals(orig_data, cleaned_data, totals_check);
        // 2.2
        locateAndTagBelowLineItems(cleaned_data);
        // 2.3
        if (cleaned_data.capacity() == 0) {
            std::cout << "Estimate Data Is Blank - Check Estimate";
            return ERROR_BLANK_ESTIM_DATA;
        }
        deleteUnusedAreaData(cleaned_data);
        // 2.4
        print_ed(cleaned_data);

    // 3. Unpivot the Cost Data for each area placing any blank data in area 0
    std::vector<unpivot_area_data> upv;
    // 3.1
    calculateUnpivotAreaData(cleaned_data, upv);

    // 4. Check the totals match the totals within the estimte sheet calculation
    checkTotals(cleaned_data, upv, totals_check);

    // 5. Loop thru each estimate record and determine which spreads should be applied to the line items
    // 5.1
    populate_spread_true_false_for_records(upv, sd);
    // 5.3 Create a map to aggregate the unpivoted hours and cost data with Group by area and category
    compute_amt_to_spread_aggregates(upv, sd);
    // 5.4
    remove_spread_lines_from_unpivot_data(upv);
    // 5.5
    compute_spread_basis_aggregates(upv, sd);
    // 5.6
    compute_spread_amounts(upv, sd);
    // 5.7
    check_spread_amounts_totals(upv, sd);
    // 5.8
    // Create a vector to store the final data
    std::vector<aggregated_data> fd;
    compute_sum_base_and_spread_amounts(upv, sd, fd);

    // 6.1
    print_unpivot_data(upv, sd);
    // 6.2
    print_sd(sd);
    // 6.3
    print_final_data(fd);

    //
    std::ofstream outFile;
    //7.1 
    open_file_to_write_results_binary(outFile);
    // 7.2
    write_unpivot_data(outFile, upv, sd);
    // 7.3
    write_sd(outFile, sd);
    //7.4
    write_final_data(outFile, fd);

    return 0;
}

//1.
int readDataFromBinaryFile(std::vector<estim_data>& orig_data, std::vector<spread_items>& sd, gen_info& gi) {
    std::ifstream file("C:/t/data.bin", std::ios::binary);
    if (!file) {
        std::cerr << "Unable to open file";
        return 1;
    }
        estim_data ed;

    while (file.peek() != EOF) {        
        // Read the number of rows of data that were written in the Binary File
        file.read(reinterpret_cast<char*>(&gi.row_cnt_ed), sizeof(gi.row_cnt_ed));

        // Read the number of rows of spreads data that were written in the Binary File
        file.read(reinterpret_cast<char*>(&gi.row_cnt_sd), sizeof(gi.row_cnt_sd));

        // Read the number of areas
        file.read(reinterpret_cast<char*>(&gi.area_cnt), sizeof(gi.area_cnt));

        //Read each rows from the Binary file
        for (int i=0; i<gi.row_cnt_ed; i++) {
            readStructure1(file, ed, gi);
            orig_data.push_back(ed);
        }
        // define an instance of spread def to collect line values
        spread_items temp_sd;

        for (int i=0; i<gi.row_cnt_sd; i++) {
            readStructure2(file, temp_sd);
            sd.push_back(temp_sd);
        }
    }
    file.close();
    return 0;
}

//1.1
void readStructure1(std::ifstream& file, estim_data& s, gen_info& gi) {
    
    // Read the main estimate binary data
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
    readString(file, s.lab_type);

    s.area_qty.resize(gi.area_cnt);
    file.read(reinterpret_cast<char*>(s.area_qty.data()), gi.area_cnt * sizeof(double));
}

//1.2
void readStructure2(std::ifstream& file, spread_items& temp_sd) {
    // Now read the other small spread basis structure
    file.read(reinterpret_cast<char*>(&temp_sd.sprd_order), sizeof(temp_sd.sprd_order));
    readString(file, temp_sd.sprd_desc);
    readString(file, temp_sd.type);
    readString(file, temp_sd.div);
    readString(file, temp_sd.discp);
    readString(file, temp_sd.lab_type);
    readString(file, temp_sd.atl_btl);
    readString(file, temp_sd.sprd_basis_field);
    readString(file, temp_sd.incl_cumul_sprd_values_list);
    }

//1.3
void readString(std::ifstream &file, std::string &str) {
      // Read length as a single byte, since written as a byte 0-255 numeric in vba
    unsigned char length;
    file.read(reinterpret_cast<char*>(&length), sizeof(length));
    str.resize(length);
    file.read(&str[0], length);
}

//2.1
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

//2.2
int locateAndTagBelowLineItems(std::vector<estim_data>& cleaned_data) {
    // Copy elements with the sum of labor, matl, sub, and eq not equal to zero

    int btl_start_indx;
    int indx = 0;
    for (const auto& cd : cleaned_data) {
       if (cd.desc == "Margin-Calc. Amount") {
            btl_start_indx = indx + 1;
            break;
        }
        indx ++;
    }

    auto it_ed = cleaned_data.begin() + btl_start_indx;

    for (it_ed; it_ed < cleaned_data.end(); ++it_ed ) {
        it_ed->atl_btl = "b";
    }

    return 0;
}

//2.3
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

//2.4
void print_ed(std::vector<estim_data>& data) {
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
    << std::setw(8) << "atl_btl" << "|"
    << std::setw(9) << "lab_type" << "|";
    int i=0;
    for (const auto& s : data) {
        for (const auto &val : s.area_qty) {
            i++;
            std::cout << std::setw(10) << "Area " + std::to_string(i) << "|" ;
        }
        std::cout << "\n";
        break;
    }
    
    // Output data to screen verify the read of the data
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
            << std::setw(8) << s.atl_btl << "|"
            << std::setw(9) << s.lab_type << "|" ;
        for (const auto &val : s.area_qty) {
            std::cout << std::setw(10) << val << "|" ;
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}


//3.1
void calculateUnpivotAreaData(std::vector<estim_data>& cleaned_data, std::vector<unpivot_area_data>& upv) {
    
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

        for (auto& a : d.area_qty) {
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
                    a = 1; // If the area sum is equal to zero then set the qty in area_0 = 1
                }
            }
            frac_total = frac_labor + frac_matl + frac_sub + frac_eq;

            upv.push_back( { areaItt, d.rowNo, d.brkd_ref, frac_other_mh, a, frac_mh_tot, frac_labor, frac_matl, frac_sub, frac_eq, 
                            frac_total, d.type, d.div, d.discp, d.lab_type, d.atl_btl } );
            
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
}

// 4.
void checkTotals(std::vector<estim_data>& cleaned_data, std::vector<unpivot_area_data>& upv, std::vector<estim_data>& totals_check) {
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
    totals_check[2].other_mh = upv.back().other_mh;
    totals_check[2].mh_tot = upv.back().mh_tot;
    totals_check[2].labor = upv.back().labor;
    totals_check[2].matl = upv.back().matl;
    totals_check[2].sub = upv.back().sub;
    totals_check[2].eq = upv.back().eq;

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

// 5.1
void populate_spread_true_false_for_records(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd) {

    // iterator thru each record of the estimate data
    for (auto& up : upv) {
        
        // resize the ectors to match the number of spreads in the sd vector
        up.apply_TF.resize(sd.size(), false);  //Initialized to false and reset as needed
        up.sprd_lab.resize(sd.size() , 0.0);  //Initialized to zero and reset as needed
        up.sprd_mat.resize(sd.size() , 0.0);  //Initialized to zero and reset as needed
        up.sprd_sub.resize(sd.size() , 0.0);  //Initialized to zero and reset as needed
        up.sprd_eqp.resize(sd.size() , 0.0);  //Initialized to zero and reset as needed
        // std::vector<double> sprd_lab;


        // create iterator for std::vector<bool> apply_TF that is inside the estimate data vector 'ed' 
        auto it_tf = up.apply_TF.begin();
        // create iterator for the 'sd' vector which contains 'sb' nested vector with fields cost_type, div, discipline, lab_type
        auto it_spr = sd.begin();
        
        for (it_tf; it_tf < up.apply_TF.end(); ++it_tf) {
            if (is_str_match_w_wildcard(up.type, it_spr->type) &&
                is_str_match_w_wildcard(up.div, it_spr->div) &&
                is_str_match_w_wildcard(up.discp, it_spr->discp) &&
                is_str_match_w_wildcard(up.lab_type, it_spr->lab_type) &&
                is_str_match_w_wildcard(up.atl_btl, it_spr->atl_btl)) {
                *it_tf = true;
            }
            it_spr++;
        }
    }
}

// 5.2
bool is_str_match_w_wildcard(std::string& str1, std::string& str2) {
    if (str1 == str2) {
        return true;
    }
    if (str2 == "*") {
        return true;
    }
    return false;
}

// 5.3
void compute_amt_to_spread_aggregates(const std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd ) {

    // create a map to aggregate the unpivot data
    std::map<std::string, aggregated_data> aggr_sd;

    for (auto up : upv) {
        if (up.brkd_ref.rfind("Spread", 0) == 0) {
            aggr_sd[up.brkd_ref].other_mh += up.other_mh;
            aggr_sd[up.brkd_ref].mh_tot += up.mh_tot;
            aggr_sd[up.brkd_ref].area_qty += up.area_qty;
            aggr_sd[up.brkd_ref].labor += up.labor;
            aggr_sd[up.brkd_ref].matl += up.matl;
            aggr_sd[up.brkd_ref].sub += up.sub;
            aggr_sd[up.brkd_ref].eq += up.eq;
        }
    }

    // transfer the aggregate calculted amounts from the map created above to the vector passed into the function by reference
    for (auto& s : sd) {
        s.amt_to_spr_l = aggr_sd[s.sprd_desc].labor;
        s.amt_to_spr_m = aggr_sd[s.sprd_desc].matl;
        s.amt_to_spr_s = aggr_sd[s.sprd_desc].sub;
        s.amt_to_spr_e = aggr_sd[s.sprd_desc].eq;
    }

    std::cout << "\nCheck transfer values of amt to spread aggregates\n";
    std::cout << std::setw(8) << "i" 
                << std::setw(30) << "sprd_desc" 
                << std::setw(10) << "lab" 
                << std::setw(10) << "mat" 
                << std::setw(10) << "sub"
                << std::setw(10) << "ep" << "\n";
    
    int i = 0;
    for (auto& s : sd) {
        std::cout   << std::setw( 8) << i
                    << std::setw(30) << s.sprd_desc
                    << std::setw(10) << s.amt_to_spr_l
                    << std::setw(10) << s.amt_to_spr_m
                    << std::setw(10) << s.amt_to_spr_s
                    << std::setw(10) << s.amt_to_spr_e << "\n";
        i++;
    }

}

/*5.4*/
void remove_spread_lines_from_unpivot_data(std::vector<unpivot_area_data>& upv) {

//Create temporary Vector the does not contain any of the spreads, then it will be reassigned to the original unpivot vector
std::vector<unpivot_area_data> tmp_upv;

    for (auto& up : upv) {
        if ( up.brkd_ref.rfind("Spread", 0) != 0 ) {
            tmp_upv.push_back(up);
        }
    }
    // Now reassign or overwrite the unpivot vector with the filtered temporary data 
    upv = tmp_upv;
}


//5.5
void compute_spread_basis_aggregates(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd) {
    // create a map to aggregate the unpivot data
    int spread_id = 0;
    std::map<int, aggregated_data> aggr_sd_basis;
    
    //start iteration throught the unpivoted data rows
    for (auto& up : upv) {
        //next start an iterator to cycle thru the different boolean values
        auto it_tf = up.apply_TF.begin();

        spread_id = 0;
        for (it_tf; it_tf < up.apply_TF.end(); ++it_tf) {
            if ( *it_tf ) {
                aggr_sd_basis[spread_id].other_mh += up.other_mh;
                aggr_sd_basis[spread_id].mh_tot += up.mh_tot;
                aggr_sd_basis[spread_id].area_qty += up.area_qty;
                aggr_sd_basis[spread_id].labor += up.labor;
                aggr_sd_basis[spread_id].matl += up.matl;
                aggr_sd_basis[spread_id].sub += up.sub;
                aggr_sd_basis[spread_id].eq += up.eq;
            }
            spread_id++;
        }
    }

    // transfer the aggregate calculted amounts from the map created above to the 'sd' vector passed into the function by reference
// **********************************************************************************
// fix mhrs later
// **********************************************************************************
// **********************************************************************************
    spread_id = 0;
    for (auto& s : sd) {
        s.spr_l_agr = aggr_sd_basis[spread_id].labor;
        s.spr_m_agr = aggr_sd_basis[spread_id].matl;
        s.spr_s_agr = aggr_sd_basis[spread_id].sub;
        s.spr_e_agr = aggr_sd_basis[spread_id].eq;
        spread_id++;
    }

}


// 5.6
void compute_spread_amounts(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd) {

    std::string sprd_basis_field;
    int case_sel = 0;

    for (auto& up : upv) {
        //setup iterators to travere the respective categories within the 'up' vector item
        auto it_sd = sd.begin();
        auto it_tf = up.apply_TF.begin();
        auto it_sd_l = up.sprd_lab.begin();
        auto it_sd_m = up.sprd_mat.begin();
        auto it_sd_s = up.sprd_sub.begin();
        auto it_sd_e = up.sprd_eqp.begin();
        
        
        while ( it_sd != sd.end() &&
                it_tf != up.apply_TF.end() &&
                it_sd_l != up.sprd_lab.end() &&
                it_sd_m != up.sprd_mat.end() &&
                it_sd_s != up.sprd_sub.end() &&
                it_sd_e != up.sprd_eqp.end() ) {
                
            if ( *it_tf ) {
                // determine which currect spread basis case
                if( it_sd->sprd_basis_field == "Lab") case_sel = 1;
                if( it_sd->sprd_basis_field == "Mat") case_sel = 2;
                if( it_sd->sprd_basis_field == "Sub") case_sel = 3;
                if( it_sd->sprd_basis_field == "Eqp") case_sel = 4;
                if( it_sd->sprd_basis_field == "All") case_sel = 5;

                double sprd_fraction[4];
                switch (case_sel)
                {
                    case 1: {
                        if (it_sd->spr_l_agr > 0.000001) {
                            sprd_fraction[0] = up.labor / it_sd->spr_l_agr;
                        } else {
                            sprd_fraction[0] = 0;
                        }
                        break;
                    }
                    case 2: {
                        if (it_sd->spr_m_agr > 0.000001) { 
                            sprd_fraction[0] = up.matl / it_sd->spr_m_agr;
                        } else {
                            sprd_fraction[0] = 0;
                        }
                        break;
                    }
                    case 3: {
                        if (it_sd->spr_s_agr > 0.000001) {
                            sprd_fraction[0] = up.sub / it_sd->spr_s_agr;
                        } else {
                            sprd_fraction[0] = 0;
                        }
                        break;
                    }
                    case 4: {
                        if (it_sd->spr_e_agr > 0.000001) {
                            sprd_fraction[0] = up.eq / it_sd->spr_e_agr;
                        } else {
                            sprd_fraction[0] = 0;
                        }
                        break;
                    }
                    case 5: {
                        if (it_sd->spr_l_agr > 0.000001) sprd_fraction[0] = up.labor / it_sd->spr_l_agr;
                        if (it_sd->spr_m_agr > 0.000001) sprd_fraction[1] = up.matl / it_sd->spr_m_agr;
                        if (it_sd->spr_s_agr > 0.000001) sprd_fraction[2] = up.sub / it_sd->spr_s_agr;
                        if (it_sd->spr_e_agr > 0.000001) sprd_fraction[3] = up.eq / it_sd->spr_e_agr;
                        break;
                    }
                    default:
                        break;
                }
                if (case_sel == 5 ) {
                    // use varying fraction for respective lab/mat/sub/eqp amounts for case 5 only
                    if (it_sd->spr_l_agr > 0.000001) *it_sd_l = it_sd->amt_to_spr_l * sprd_fraction[0];
                    if (it_sd->spr_m_agr > 0.000001) *it_sd_m = it_sd->amt_to_spr_m * sprd_fraction[1];
                    if (it_sd->spr_s_agr > 0.000001) *it_sd_s = it_sd->amt_to_spr_s * sprd_fraction[2];
                    if (it_sd->spr_e_agr > 0.000001) *it_sd_e = it_sd->amt_to_spr_e * sprd_fraction[3];
                } else {
                    // use fraction calculated and placed at the [0] element
                    if (sprd_fraction[0] > 0.000001) *it_sd_l = it_sd->amt_to_spr_l * sprd_fraction[0];
                    if (sprd_fraction[0] > 0.000001) *it_sd_m = it_sd->amt_to_spr_m * sprd_fraction[0];
                    if (sprd_fraction[0] > 0.000001) *it_sd_s = it_sd->amt_to_spr_s * sprd_fraction[0];
                    if (sprd_fraction[0] > 0.000001) *it_sd_e = it_sd->amt_to_spr_e * sprd_fraction[0];
                }
            }
            it_sd++;
            it_tf++;
            it_sd_l++;
            it_sd_m++;
            it_sd_s++;
            it_sd_e++;
        }
    }
}

// 6.1
void print_unpivot_data(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd) {
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
                << std::setw(10) << "total" << "|"
                << std::setw(10) << "type" << "|"
                << std::setw(10) << "div" << "|"
                << std::setw(10) << "discp" << "|"
                << std::setw(10) << "lab_type" << "|"
                << std::setw(8) << "atl_btl" << "|";
        int i=0;    
        for (const auto s : sd) {
            std::cout << std::setw(8) << "Cond_" + std::to_string(i) << "|";
            i++;
        }
        i=0;
        for (const auto s : sd) {
            std::cout << std::setw(10) << "Spr_l_" + std::to_string(i) << "|";
            std::cout << std::setw(10) << "Spr_m_" + std::to_string(i) << "|";
            std::cout << std::setw(10) << "Spr_s_" + std::to_string(i) << "|";
            std::cout << std::setw(10) << "Spr_e_" + std::to_string(i) << "|";
            i++;
        }
        std::cout << "\n";

    for (const auto& up : upv) {
        std::cout   << std::setw( 6) << std::left << up.areaNo << "|"
                    << std::setw( 6) << up.rowNo << "|"
                    << std::setw(42) << up.brkd_ref  << "|"
                    << std::setw(10) << up.other_mh << "|"
                    << std::setw(10) << up.area_qty << "|"
                    << std::setw(10) << up.mh_tot << "|"
                    << std::setw(10) << up.labor << "|"
                    << std::setw(10) << up.matl << "|"
                    << std::setw(10) << up.sub << "|"
                    << std::setw(10) << up.eq << "|"
                    << std::setw(10) << up.total << "|"
                    << std::setw(10) << up.type << "|"
                    << std::setw(10) << up.div << "|"
                    << std::setw(10) << up.discp << "|"
                    << std::setw(10) << up.lab_type << "|"
                    << std::setw(8) << up.atl_btl << "|";
        
        for (auto it_tf = up.apply_TF.begin(); it_tf < up.apply_TF.end(); it_tf++) {
            std::cout << std::setw(8) << std::boolalpha << *it_tf << "|";
        }
        
        // Next iterate thru the spread values and print
        auto it_sl = up.sprd_lab.begin();
        auto it_sm = up.sprd_mat.begin();
        auto it_ss = up.sprd_sub.begin();
        auto it_se = up.sprd_eqp.begin();
        while ( it_sl != up.sprd_lab.end() && 
                it_sm != up.sprd_mat.end() && 
                it_ss != up.sprd_sub.end() && 
                it_se != up.sprd_eqp.end() ) {
            std::cout << std::fixed << std::setprecision(2) << std::setw(10) << *it_sl << "|";
            std::cout << std::fixed << std::setprecision(2) << std::setw(10) << *it_sm << "|";
            std::cout << std::fixed << std::setprecision(2) << std::setw(10) << *it_ss << "|";
            std::cout << std::fixed << std::setprecision(2) << std::setw(10) << *it_se << "|";
            it_sl++;
            it_sm++;
            it_ss++;
            it_se++;
        }
    std::cout << "\n";
    }
}

//5.6
void check_spread_amounts_totals(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd) {

    std::vector<aggregated_data> chk_agr;
    for (auto& s : sd) {
        chk_agr.resize(sd.size(), { 0, "", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 } );
    }

    for (auto& up : upv) {
        //setup iterators to travere the respective categories within the 'up' vector item
        auto it_chk_agr = chk_agr.begin();
        auto it_sd_l = up.sprd_lab.begin();
        auto it_sd_m = up.sprd_mat.begin();
        auto it_sd_s = up.sprd_sub.begin();
        auto it_sd_e = up.sprd_eqp.begin();

        while ( it_chk_agr != chk_agr.end() &&
                it_sd_l != up.sprd_lab.end() &&
                it_sd_m != up.sprd_mat.end() &&
                it_sd_s != up.sprd_sub.end() &&
                it_sd_e != up.sprd_eqp.end() ) {

            it_chk_agr->labor += *it_sd_l;
            it_chk_agr->matl += *it_sd_m;
            it_chk_agr->sub += *it_sd_s;
            it_chk_agr->eq += *it_sd_e;

            it_chk_agr++;
            it_sd_l++;
            it_sd_m++;
            it_sd_s++;
            it_sd_e++;
        }
    }

    std::cout << "\ncheck data\n";

    for (auto& agr : chk_agr) {
    std::cout   << agr.labor << "|"
                << agr.matl << "|" 
                << agr.sub << "|" 
                << agr.eq << "\n";
    }
}


//5.7
void compute_sum_base_and_spread_amounts(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd, std::vector<aggregated_data>& fd) {

    // Create a map to aggregate the unpivoted hours and cost data with Group by area and category
    std::map<int, std::map<std::string, aggregated_data>> aggr_map_by_area_categ;

    for (auto& up : upv) {
        
        // Use the Map to gather the aggregates first for the base amounts
        aggr_map_by_area_categ[up.areaNo][up.brkd_ref].labor += up.labor;
        aggr_map_by_area_categ[up.areaNo][up.brkd_ref].matl += up.matl;
        aggr_map_by_area_categ[up.areaNo][up.brkd_ref].sub += up.sub;
        aggr_map_by_area_categ[up.areaNo][up.brkd_ref].eq += up.eq;

        // Sum up all the spreads and add to the lab,mat,sub,eqp amounts above for each 'row'        
        auto it_sl = up.sprd_lab.begin();
        auto it_sm = up.sprd_mat.begin();
        auto it_ss = up.sprd_sub.begin();
        auto it_se = up.sprd_eqp.begin();

        while ( it_sl != up.sprd_lab.end() && 
                it_sm != up.sprd_mat.end() && 
                it_ss != up.sprd_sub.end() && 
                it_se != up.sprd_eqp.end() ) {

                // Use the Map to gather the aggregates first for the base amounts
                aggr_map_by_area_categ[up.areaNo][up.brkd_ref].labor += *it_sl;
                aggr_map_by_area_categ[up.areaNo][up.brkd_ref].matl += *it_sm;
                aggr_map_by_area_categ[up.areaNo][up.brkd_ref].sub += *it_ss;
                aggr_map_by_area_categ[up.areaNo][up.brkd_ref].eq += *it_se;

            it_sl++;
            it_sm++;            
            it_ss++;
            it_se++;                        
        }
    }

    double tmp_total = 0;    
    for (auto& area_pair : aggr_map_by_area_categ) {
        for (auto& brkd_pair : area_pair.second) {
            tmp_total = brkd_pair.second.labor + brkd_pair.second.matl + brkd_pair.second.sub + brkd_pair.second.eq;
            fd.push_back( { area_pair.first, brkd_pair.first, brkd_pair.second.other_mh, brkd_pair.second.mh_tot, 
                            brkd_pair.second.area_qty, brkd_pair.second.labor, brkd_pair.second.matl, brkd_pair.second.sub, brkd_pair.second.eq, tmp_total} );
        }
    }
} 


// 6.1
void print_sd(std::vector<spread_items>& sd) {

    // Print Titles    
    std::cout << "\n";
    std::cout << std::setw(12) <<  std::left << "sprd_order" << "|"
    << std::setw(30) <<  std::left << "sprd_desc" << "|"
    << std::setw(10) << "type" << "|" 
    << std::setw(10) << "div" << "|" 
    << std::setw(10) << "discp" << "|" 
    << std::setw(10) << "lab_type" << "|" 
    << std::setw(10) << "atl_btl" << "|"
    << std::setw(18) << "sprd_basis_field" << "|"
    << std::setw(30) << "incl_cumul_sprd_values_list" << "|"
    << std::setw(14) << "amt_to_spr_l" << "|" 
    << std::setw(14) << "amt_to_spr_m" << "|" 
    << std::setw(14) << "amt_to_spr_s" << "|" 
    << std::setw(14) << "amt_to_spr_e" << "|" 
    << std::setw(10) << "spr_l_agr" << "|" 
    << std::setw(10) << "spr_m_agr" << "|" 
    << std::setw(10) << "spr_s_agr" << "|" 
    << std::setw(10) << "spr_e_agr" << "\n";
    
    // Output to screen verify the read of the data
    for (const auto& s : sd) {
        std::cout << std::setw(12) << std::left << s.sprd_order << "|" 
            << std::setw(30) << std::left << s.sprd_desc << "|" 
            << std::setw(10) << s.type << "|" 
            << std::setw(10) << s.div << "|" 
            << std::setw(10) << s.discp << "|" 
            << std::setw(10) << s.lab_type << "|" 
            << std::setw(10) << s.atl_btl << "|"
            << std::setw(18) << s.sprd_basis_field << "|"
            << std::setw(30) << s.incl_cumul_sprd_values_list << "|"
            << std::setw(14) << s.amt_to_spr_l << "|"
            << std::setw(14) << s.amt_to_spr_m << "|"
            << std::setw(14) << s.amt_to_spr_s << "|"
            << std::setw(14) << s.amt_to_spr_e << "|"
            << std::setw(10) << s.spr_l_agr << "|"
            << std::setw(10) << s.spr_m_agr << "|"
            << std::setw(10) << s.spr_s_agr << "|"
            << std::setw(10) << s.spr_e_agr << "\n";
    }
    std::cout << std::endl;
}

// 6.3
void print_final_data(std::vector<aggregated_data>& fd) {
        int areaNo;
    std::string brkd_ref;
    double other_mh = 0;
    double mh_tot = 0;
    double area_qty = 0;
    double labor = 0;
    double matl = 0;
    double sub = 0;
    double eq = 0;
    double total = 0;

    // Print Titles    
    std::cout << "\n";
    std::cout << std::setw(10) <<  std::left << "areaNo" << "|"
    << std::setw(30) << "brkd_ref" << "|" 
    << std::setw(10) << "other_mh" << "|" 
    << std::setw(10) << "mh_tot" << "|" 
    << std::setw(10) << "area_qty" << "|" 
    << std::setw(10) << "labor" << "|"
    << std::setw(10) << "matl" << "|" 
    << std::setw(10) << "sub" << "|" 
    << std::setw(10) << "eq" << "|" 
    << std::setw(10) << "total" << "\n"; 

    
    // Output to screen verify the read of the data
    int prevAreaNo = 0;
    for (const auto& f : fd) {

        if(f.areaNo != prevAreaNo ) {
            std::cout << "\n";
        }
        std::cout << std::setw(10) << std::left << f.areaNo << "|" 
            << std::setw(30) << f.brkd_ref << "|" 
            << std::setw(10) << f.other_mh << "|"
            << std::setw(10) << f.mh_tot << "|"
            << std::setw(10) << f.area_qty << "|"
            << std::setw(10) << f.labor << "|"
            << std::setw(10) << f.matl << "|"
            << std::setw(10) << f.sub << "|"
            << std::setw(10) << f.eq << "|"
            << std::setw(10) << f.total << "\n";
            prevAreaNo = f.areaNo;
    }
    std::cout << std::endl;
}

//7. Write Binary Output File for Excel Import
    /*7.1*/ void open_file_to_write_results_binary(std::ofstream& outFile) {
        const std::string filename = "c:/t/brkd_output.bin";

        // Remove the file if it exists
        if (std::remove(filename.c_str()) == 0) {
            std::cout << "Previous file deleted." << std::endl;
        }


        outFile.open(filename, std::ios::binary);
        if (!outFile) {
            std::cerr << "Error opening file for writing!" << std::endl;
        }
        
        // ****************************************************************************************************
        // ****************** move this to end after writing all data outfile after writing *******************
        // ****************************************************************************************************
        // ****************************************************************************************************
        outFile.close();
        std::cout << "File closed after writing final data." << std::endl;

    }
    /*7.2*/ void write_unpivot_data(std::ofstream& outFile, std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd) {

        unsigned short no_of_rows = upv.size();


    }
    /*7.3*/ void write_sd(std::ofstream& outFile, std::vector<spread_items>& sd) {

    }
    /*7.4*/ void write_final_data(std::ofstream& outFile, std::vector<aggregated_data>& fd){

    }

//20.1
std::string formatCurrency(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << "$" << value;
    return oss.str();

//20.2
}
std::string formatFixed(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    return oss.str();

//20.3
}
double roundToTwoDecimalPlaces(double value) {
    return std::round(value * 100.0) / 100.0;

//20.4
}

bool isZero(double chk_value) {
    double epsilon = 1e-10;
    return std::fabs(chk_value) < epsilon;
}

// 20.5
void printAnyErrors(int result, int estim_row) {
    if (result != SUCCESS) {
        // Handle errors
        switch (result) {
            case ERROR_BLANK_ESTIM_DATA:
                std::cerr << "Error: Estimate Data Appears to be Blank\n";
                break;
            default:
                std::cerr << "Error: Unknown error occurred.\n";
        }
    } else {
        std::cout << "Run Completed without any known errors.\n";
    }
}