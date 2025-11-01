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
    double amt_to_spr_omh;       // Amount to Spread omh
    double amt_to_spr_mh;       // Amount to Spread omh
    double amt_to_spr_qty;       // Amount to Spread omh
    double amt_to_spr_l;            // Amount to Spread Labor
    double amt_to_spr_m;    
    double amt_to_spr_s;
    double amt_to_spr_e;
    double spr_omh_agr;      // spread basis aggregate
    double spr_mh_agr;
    double spr_qty_agr;
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
    double omh;
    std::string param1;
    std::string param2;
    std::string param3;
    double qty;
    std::string uom;
    double umh;
    double mh;
    double rate;
    double lab;
    double mat;
    double sub;
    double eqp;
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
    int areaNo;     // cooresponds to a long in vba
    int rowNo;     // cooresponds to a long in vba
    std::string brkd_ref;
    double omh;
    double area_qty;
    std::string uom;
    double mh;
    double lab;
    double mat;
    double sub;
    double eqp;
    double total;
    std::string type;
    std::string div;
    std::string discp;
    std::string lab_type;
    std::string atl_btl = "a";    // Plaeholder for above the Line (atl) or Below the Line (btl)
    std::vector<bool> apply_TF;
    std::vector<double> sprd_omh;
    std::vector<double> sprd_mh;
    std::vector<double> sprd_lab;
    std::vector<double> sprd_mat;
    std::vector<double> sprd_sub;
    std::vector<double> sprd_eqp;
};

struct final_results {
    int areaNo;
    std::string brkd_ref;
    double omh = 0;
    double mh = 0;
    double area_qty = 0;
    double lab = 0;
    double mat = 0;
    double sub = 0;
    double eqp = 0;
    double total = 0;
};

// Function Prototypes
/* 1.*/ int readDataFromBinaryFile(std::vector<estim_data>& orig_data, std::vector<spread_items>& sd, gen_info& gi);
    /*1.1*/ void readStructure1(std::ifstream& file, estim_data& s, gen_info& gen_info);
    /*1.2*/ void readStructure2(std::ifstream& file, spread_items& temp_sd);
    /*1.3*/ void readString(std::ifstream &file, std::string &str);

// 2. Now cleanup the data by removing zero lines, and also brkd_ref item = "-"
    /*2.1*/ int filterOutDataRemoveZerosAndTotals(std::vector<estim_data>& orig_data, std::vector<estim_data>& cleaned_data, std::vector<estim_data>& totals_check);
    /*2.2*/ int locateAndTagBelowLineItems(std::vector<estim_data>& cleaned_data);
    /*2.3*/ void deleteUnusedAreaData(std::vector<estim_data>& data);
    /*2.4*/ void print_ed(std::vector<estim_data>& orig_data);

/* 3.*/ void calculateUnpivotAreaData(std::vector<estim_data>& cleaned_data, std::vector<unpivot_area_data>& upv);

/* 4.*/  void checkTotals(std::vector<estim_data>& cleaned_data, std::vector<unpivot_area_data>& upv, std::vector<estim_data>& totals_check);

// 5. Loop thru each estimate record and determine which spreads should be applied to the line items
    /*5.1*/ void populate_spread_true_false_for_records(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd);
    /*5.2*/ void compute_amt_to_spread_aggregates(const std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd );
    /*5.3*/ void remove_spread_lines_from_unpivot_data(std::vector<unpivot_area_data>& upv);
    /*5.4*/ void calc_spread_basis_aggregates_stepA(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd);
    /*5.5*/ void calc_spread_amounts_stepA(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd);

    /*5.6*/ void recalc_spread_amounts_stepB(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd);

    
    /*5.7*/ void check_spread_amounts_totals(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd);
    /*5.8*/ void compute_sum_base_and_spread_amounts(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd, std::vector<final_results>& fd);
    /*5.9*/ void create_aggregate_all_areas(std::vector<final_results>& fd);

// 6. Print Results to Screen
    /*6.1*/ void print_unpivot_data(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd);
    /*6.2*/ void print_sd(std::vector<spread_items>& sd);
    /*6.3*/ void print_final_data(std::vector<final_results>& fd);

// 7. Write Binary Output File for Excel Import
    /*7.1*/ void open_file_to_write_results_binary(std::ofstream& outFile);
    /*7.2*/ void writeStr(std::ofstream &file, std::string &str);
    /*7.3*/ void writeDbl(std::ofstream& file, double & num);
    /*7.4*/ void writeShr(std::ofstream& file, unsigned short& num);
    /*7.5*/ void writeInt(std::ofstream& file, int& num);
    /*7.6*/ void writeLong(std::ofstream& file, long& num);
    /*7.7*/ void write_unpivot_data(std::ofstream& outFile, std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd);
    /*7.8*/ void write_sd(std::ofstream& outFile, std::vector<spread_items>& sd);
    /*7.9*/ void write_final_data(std::ofstream& outFile, std::vector<final_results>& fd);

// 20. Misc Small Helper Functions
    /*20.1*/ std::string formatCurrency(double value);
    /*20.2*/ std::string formatFixed(double value);
    /*20.3*/ double roundToTwoDecimalPlaces(double value);
    /*20.4*/ bool isZero(double chk_value);
    /*20.5*/ void printAnyErrors(int result, int estim_row);
    /*20.6*/ std::string boolToString(bool value);
    /*20.7*/ bool starts_with_key(const std::string& str);
    /*20.8*/ bool is_str_match_w_wildcard(std::string& str1, std::string& str2);


// *********************************************************
// *********************  I AM MAIN ************************
// *********************************************************
int main(int argc, char const *argv[]) {

    // 0. Declare key structs
    std::vector<estim_data> orig_data;
    std::vector<spread_items> sd;
    gen_info gi;

    // 1. Read in original data from the binary and print to screen for reference    
    readDataFromBinaryFile(orig_data, sd, gi);
    // print_ed(orig_data);

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
    calculateUnpivotAreaData(cleaned_data, upv);

    // 4. Check the totals match the totals within the estimte sheet calculation
    checkTotals(cleaned_data, upv, totals_check);

    // 5. Loop thru each estimate record and determine which spreads should be applied to the line items
    // 5.1
    populate_spread_true_false_for_records(upv, sd);
    // 5.2 Create a map to aggregate the unpivoted hours and cost data with Group by area and category
    compute_amt_to_spread_aggregates(upv, sd);
    // 5.3
    remove_spread_lines_from_unpivot_data(upv);
    // 5.4
    calc_spread_basis_aggregates_stepA(upv, sd);
    // 5.5
    calc_spread_amounts_stepA(upv, sd);
    
    // 5.6
    // ********************** Fix this later *******************************
    // recalc_spread_amounts_stepB(upv, sd);

    // 5.7
    check_spread_amounts_totals(upv, sd);
    // 5.8 Create a vector to store the final data and Sum All Amounts
    std::vector<final_results> fd;
    compute_sum_base_and_spread_amounts(upv, sd, fd);
    // 5.9
    create_aggregate_all_areas(fd);

    // 6. Screen Print Data
    // 6.1
    print_unpivot_data(upv, sd);
    // 6.2
    print_sd(sd);
    // 6.3
    print_final_data(fd);

    // 7. Write Results to Binary File
    std::ofstream outFile;
    // 7.1 
    open_file_to_write_results_binary(outFile);
    // 7.2
    write_unpivot_data(outFile, upv, sd);
    // 7.3
    write_sd(outFile, sd);
    // 7.4
    write_final_data(outFile, fd);

    // Pause the console window before exiting
    system("pause");

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
    file.read(reinterpret_cast<char*>(&s.omh), sizeof(s.omh));
    readString(file, s.param1);
    readString(file, s.param2);
    readString(file, s.param3);
    readString(file, s.type);
    file.read(reinterpret_cast<char*>(&s.qty), sizeof(s.qty));
    readString(file, s.uom);
    file.read(reinterpret_cast<char*>(&s.umh), sizeof(s.umh));
    file.read(reinterpret_cast<char*>(&s.mh), sizeof(s.mh));
    file.read(reinterpret_cast<char*>(&s.rate), sizeof(s.rate));
    file.read(reinterpret_cast<char*>(&s.lab), sizeof(s.lab));
    file.read(reinterpret_cast<char*>(&s.mat), sizeof(s.mat));
    file.read(reinterpret_cast<char*>(&s.sub), sizeof(s.sub));
    file.read(reinterpret_cast<char*>(&s.eqp), sizeof(s.eqp));
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
    // Copy elements with the sum of lab, mat, sub, and eqp not equal to zero
    for (const auto& ed : orig_data) {
        if ((ed.lab + ed.mat + ed.sub + ed.eqp) != 0  && ed.brkd_ref != "-" ) {
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
    // Copy elements with the sum of lab, mat, sub, and eqp not equal to zero

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
    << std::setw(9) << "omh" << "|" 
    << std::setw(7) << "param1" << "|" 
    << std::setw(7) << "param2" << "|" 
    << std::setw(10) << "param3" << "|" 
    << std::setw(5) << "type" << "|" 
    << std::setw(8) << "qty" << "|" 
    << std::setw(10) << "uom" << "|" 
    << std::setw(10) << "umh" << "|" 
    << std::setw(10) << "mh" << "|" 
    << std::setw(10) << "rate" << "|" 
    << std::setw(12) << "lab" << "|" 
    << std::setw(12) << "mat" << "|" 
    << std::setw(12) << "sub" << "|" 
    << std::setw(12) << "eqp" << "|" 
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
            << std::setw(9) << s.omh << "|" 
            << std::setw(7) << s.param1 << "|" 
            << std::setw(7) << s.param2 << "|" 
            << std::setw(10) << s.param3 << "|" 
            << std::setw(5) << s.type << "|" 
            << std::setw(8) << s.qty << "|" 
            << std::setw(10) << s.uom << "|" 
            << std::setw(10) << formatFixed(s.umh) << "|" 
            << std::setw(10) << formatFixed(s.mh) << "|" 
            << std::setw(10) << formatFixed(s.rate) << "|" 
            << std::setw(12) << formatFixed(s.lab) << "|" 
            << std::setw(12) << formatFixed(s.mat) << "|" 
            << std::setw(12) << formatFixed(s.sub) << "|" 
            << std::setw(12) << formatFixed(s.eqp) << "|" 
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

//3.
void calculateUnpivotAreaData(std::vector<estim_data>& cleaned_data, std::vector<unpivot_area_data>& upv) {
    
    //initialize some temp variables to keep running total of by area for checking purposes    
    double sum_omh = 0;
    double sum_mh = 0;
    double sum_lab = 0;
    double sum_mat = 0;
    double sum_sub = 0;
    double sum_eqp = 0;
    double sum_total = 0;

    //Iterate through the cost data and generate the unpivotated data which will be pushed back to the 'unp' vector of struct <unpivot_area_data>
    for (auto& d1 : cleaned_data) {
        for (const auto& atmp : d1.area_qty) {
            d1.areaSum += atmp;
        }
    }

    //Iterate through the cost data and generate the unpivotated data which will be pushed back to the 'unp' vector of struct <unpivot_area_data>
    for (auto& d : cleaned_data) {
        int areaIndx=0;

        for (auto& a : d.area_qty) {
            //set all temp fractional calcs equal to zero before each iteration
            double area_frac = 0;  double area_qty = 0; double frac_omh = 0; double frac_mh = 0; double frac_lab = 0; double frac_mat = 0; double frac_sub = 0; double frac_eqp = 0; double frac_total=0;

            if(!isZero(d.areaSum)) {    //Use a function to see if double is close to zero
                area_frac = a / d.areaSum;
                frac_omh = d.omh * area_frac;
                frac_mh = d.mh * area_frac;
                frac_lab = d.lab * area_frac;
                frac_mat = d.mat * area_frac;
                frac_sub = d.sub * area_frac;
                frac_eqp = d.eqp * area_frac;
            } else {
                if(areaIndx == 0) {
                    frac_omh = d.omh;  // If the area sum is equal to zero then assign all that cost to areaIndx = 0
                    frac_mh = d.mh;
                    frac_lab = d.lab;
                    frac_mat = d.mat;
                    frac_sub = d.sub;
                    frac_eqp = d.eqp;
                    a = 1; // If the area sum is equal to zero then set the qty in area_0 = 1
                }
            }
            frac_total = frac_lab + frac_mat + frac_sub + frac_eqp;

            upv.push_back( { areaIndx, d.rowNo, d.brkd_ref, frac_omh, a, d.uom, frac_mh, frac_lab, frac_mat, frac_sub, frac_eqp, 
                            frac_total, d.type, d.div, d.discp, d.lab_type, d.atl_btl } );
            
            // Compute Sum Total Checks
            sum_omh += frac_omh;
            sum_mh += frac_mh;
            sum_lab += frac_lab;
            sum_mat += frac_mat;
            sum_sub += frac_sub;
            sum_eqp += frac_eqp;
            sum_total += frac_total;
            areaIndx++;
        }
    }
}

// 4.
void checkTotals(std::vector<estim_data>& cleaned_data, std::vector<unpivot_area_data>& upv, std::vector<estim_data>& totals_check) {
    // create a placeholder for the totals that will be reset to zero and recalculated
    totals_check.push_back(totals_check.back());

    totals_check[1].omh = 0.0;
    totals_check[1].mh = 0.0;
    totals_check[1].lab = 0.0;
    totals_check[1].mat = 0.0;
    totals_check[1].sub = 0.0;
    totals_check[1].eqp = 0.0;

    // create a placeholder for the later total from unpivot data and deltas
    totals_check.push_back(totals_check.back());
    totals_check.push_back(totals_check.back());
    totals_check.push_back(totals_check.back());

    // Add up totals for checking of data
    for (const auto& ed : cleaned_data) {
        totals_check[1].omh += ed.omh;
        totals_check[1].mh += ed.mh;
        totals_check[1].lab += ed.lab;
        totals_check[1].mat += ed.mat;
        totals_check[1].sub += ed.sub;
        totals_check[1].eqp += ed.eqp;
    }
    totals_check[1].total = totals_check[1].lab + totals_check[1].mat + totals_check[1].sub + totals_check[1].eqp;

    //Get totals from Unpivot Data
    totals_check[2].omh = upv.back().omh;
    totals_check[2].mh = upv.back().mh;
    totals_check[2].lab = upv.back().lab;
    totals_check[2].mat = upv.back().mat;
    totals_check[2].sub = upv.back().sub;
    totals_check[2].eqp = upv.back().eqp;

    // calculate the deltas in totals
    for(int i=1; i<3; i++) {
    totals_check[i+2].omh = roundToTwoDecimalPlaces(totals_check[0].omh) - roundToTwoDecimalPlaces(totals_check[i].omh);
    totals_check[i+2].mh = roundToTwoDecimalPlaces(totals_check[0].mh) - roundToTwoDecimalPlaces(totals_check[i].mh);
    totals_check[i+2].lab = roundToTwoDecimalPlaces(totals_check[0].lab) - roundToTwoDecimalPlaces(totals_check[i].lab);
    totals_check[i+2].mat = roundToTwoDecimalPlaces(totals_check[0].mat) - roundToTwoDecimalPlaces(totals_check[i].mat);
    totals_check[i+2].sub = roundToTwoDecimalPlaces(totals_check[0].sub) - roundToTwoDecimalPlaces(totals_check[i].sub);
    totals_check[i+2].eqp = roundToTwoDecimalPlaces(totals_check[0].eqp) - roundToTwoDecimalPlaces(totals_check[i].eqp);
    totals_check[i+2].total = roundToTwoDecimalPlaces(totals_check[0].total) - roundToTwoDecimalPlaces(totals_check[i].total);
    }

    // Print titles
    std::cout << "\nThe following lines are a comparison of all the key Totals from the Estimate Sheet, and Checked Addition of the Total Hrs and Costs:\n";
    std::cout << std::setw(40) << std::left << "Item" 
              << std::setw(15) << std::left << "omh" 
              << std::setw(15) << std::left << "mh" 
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
                << std::setw(15) << formatFixed(totals_check[i].omh)
                << std::setw(15) << formatFixed(totals_check[i].mh)
                << std::setw(15) << formatCurrency(totals_check[i].lab)
                << std::setw(15) << formatCurrency(totals_check[i].mat)
                << std::setw(15) << formatCurrency(totals_check[i].sub)
                << std::setw(15) << formatCurrency(totals_check[i].eqp)
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
        up.sprd_omh.resize(sd.size() , 0.0);  //Initialized to zero and reset as needed
        up.sprd_mh.resize(sd.size() , 0.0);  //Initialized to zero and reset as needed
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
void compute_amt_to_spread_aggregates(const std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd ) {

    // create a map to aggregate the unpivot data where brkd_ref contains Spread
    std::map<std::string, final_results> aggr_sd;

    for (auto up : upv) {
        if (up.brkd_ref.rfind("Spread", 0) == 0) {
            aggr_sd[up.brkd_ref].omh += up.omh;
            aggr_sd[up.brkd_ref].mh += up.mh;
            aggr_sd[up.brkd_ref].area_qty += up.area_qty;
            aggr_sd[up.brkd_ref].lab += up.lab;
            aggr_sd[up.brkd_ref].mat += up.mat;
            aggr_sd[up.brkd_ref].sub += up.sub;
            aggr_sd[up.brkd_ref].eqp += up.eqp;
        }
    }

    // transfer the aggregate calculted amounts from the map created above to the vector Spread Data'sd' passed in by reference
    for (auto& s : sd) {
        s.amt_to_spr_omh = aggr_sd[s.sprd_desc].omh;
        s.amt_to_spr_mh = aggr_sd[s.sprd_desc].mh;
        s.amt_to_spr_l = aggr_sd[s.sprd_desc].lab;
        s.amt_to_spr_m = aggr_sd[s.sprd_desc].mat;
        s.amt_to_spr_s = aggr_sd[s.sprd_desc].sub;
        s.amt_to_spr_e = aggr_sd[s.sprd_desc].eqp;
    }

}

// 5.3
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

// 5.4
void calc_spread_basis_aggregates_stepA(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd) {
    // create a map to aggregate the unpivot data
    int spread_id = 0;
    std::map<int, final_results> aggr_sd_basis;
    
    //start iteration throught the unpivoted data rows
    for (auto& up : upv) {
        //next start an iterator to cycle thru the different boolean values
        auto it_tf = up.apply_TF.begin();

        spread_id = 0;
        for (it_tf; it_tf < up.apply_TF.end(); ++it_tf) {
            if ( *it_tf ) {
                aggr_sd_basis[spread_id].omh += up.omh;
                aggr_sd_basis[spread_id].mh += up.mh;
                aggr_sd_basis[spread_id].area_qty += up.area_qty;
                aggr_sd_basis[spread_id].lab += up.lab;
                aggr_sd_basis[spread_id].mat += up.mat;
                aggr_sd_basis[spread_id].sub += up.sub;
                aggr_sd_basis[spread_id].eqp += up.eqp;
            }
            spread_id++;
        }
    }

    // Next Loop thtu the spreads vector and transfer the aggregate calculted amounts from the map created above to the 'sd' vector passed into the function by reference
    spread_id = 0;
    for (auto& s : sd) {
        s.spr_omh_agr = aggr_sd_basis[spread_id].omh;
        s.spr_mh_agr = aggr_sd_basis[spread_id].mh;
        s.spr_l_agr = aggr_sd_basis[spread_id].lab;
        s.spr_m_agr = aggr_sd_basis[spread_id].mat;
        s.spr_s_agr = aggr_sd_basis[spread_id].sub;
        s.spr_e_agr = aggr_sd_basis[spread_id].eqp;
        spread_id++;
    }
}


// 5.5
void calc_spread_amounts_stepA(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd) {

    std::string sprd_basis_field;
    int case_sel = 0;

    for (auto& up : upv) {
        //setup iterators to travere the respective categories within the 'up' vector item
        auto it_sd = sd.begin();
        auto it_tf = up.apply_TF.begin();
        auto it_sd_omh = up.sprd_omh.begin();
        auto it_sd_mh = up.sprd_mh.begin();
        auto it_sd_l = up.sprd_lab.begin();
        auto it_sd_m = up.sprd_mat.begin();
        auto it_sd_s = up.sprd_sub.begin();
        auto it_sd_e = up.sprd_eqp.begin();
        
        while ( it_sd != sd.end() &&
                it_tf != up.apply_TF.end() &&
                it_sd_omh != up.sprd_omh.end() &&
                it_sd_mh != up.sprd_mh.end() &&
                it_sd_l != up.sprd_lab.end() &&
                it_sd_m != up.sprd_mat.end() &&
                it_sd_s != up.sprd_sub.end() &&
                it_sd_e != up.sprd_eqp.end() ) {
                
            if ( *it_tf ) {
                // determine which currect spread basis case
                if( it_sd->sprd_basis_field == "Omh") case_sel = 1;
                if( it_sd->sprd_basis_field == "Mh") case_sel = 2;
                if( it_sd->sprd_basis_field == "Lab") case_sel = 3;
                if( it_sd->sprd_basis_field == "Mat") case_sel = 4;
                if( it_sd->sprd_basis_field == "Sub") case_sel = 5;
                if( it_sd->sprd_basis_field == "Eqp") case_sel = 6;
                if( it_sd->sprd_basis_field == "All") case_sel = 7;

                double sprd_fraction[4];
                switch (case_sel)
                {
                    case 1: {
                        if (!isZero(it_sd->spr_omh_agr)) {
                            sprd_fraction[0] = up.omh / it_sd->spr_omh_agr;
                        } else {
                            sprd_fraction[0] = 0;
                        }
                        break;
                    }
                    case 2: {
                        if (!isZero(it_sd->spr_mh_agr)) {
                            sprd_fraction[0] = up.mh / it_sd->spr_mh_agr;
                        } else {
                            sprd_fraction[0] = 0;
                        }
                        break;
                    }
                    case 3: {
                        if (!isZero(it_sd->spr_l_agr)) {
                            sprd_fraction[0] = up.lab / it_sd->spr_l_agr;
                        } else {
                            sprd_fraction[0] = 0;
                        }
                        break;
                    }
                    case 4: {
                        if (!isZero(it_sd->spr_m_agr)) { 
                            sprd_fraction[0] = up.mat / it_sd->spr_m_agr;
                        } else {
                            sprd_fraction[0] = 0;
                        }
                        break;
                    }
                    case 5: {
                        if (!isZero(it_sd->spr_s_agr)) {
                            sprd_fraction[0] = up.sub / it_sd->spr_s_agr;
                        } else {
                            sprd_fraction[0] = 0;
                        }
                        break;
                    }
                    case 6: {
                        if (!isZero(it_sd->spr_e_agr)) {
                            sprd_fraction[0] = up.eqp / it_sd->spr_e_agr;
                        } else {
                            sprd_fraction[0] = 0;
                        }
                        break;
                    }
                    case 7: {
                        if (!isZero(it_sd->spr_omh_agr)) sprd_fraction[0] = up.omh / it_sd->spr_omh_agr;
                        if (!isZero(it_sd->spr_mh_agr)) sprd_fraction[1] = up.mh / it_sd->spr_mh_agr;
                        if (!isZero(it_sd->spr_l_agr)) sprd_fraction[2] = up.lab / it_sd->spr_l_agr;
                        if (!isZero(it_sd->spr_m_agr)) sprd_fraction[3] = up.mat / it_sd->spr_m_agr;
                        if (!isZero(it_sd->spr_s_agr)) sprd_fraction[4] = up.sub / it_sd->spr_s_agr;
                        if (!isZero(it_sd->spr_e_agr)) sprd_fraction[5] = up.eqp / it_sd->spr_e_agr;
                        break;
                    }
                    default:
                        break;
                }
                if (case_sel == 7 ) {
                    // use varying fraction for respective lab/mat/sub/eqp amounts for case 7 only
                    if (!isZero(it_sd->spr_omh_agr)) *it_sd_omh = it_sd->amt_to_spr_omh * sprd_fraction[0];
                    if (!isZero(it_sd->spr_mh_agr)) *it_sd_mh = it_sd->amt_to_spr_mh * sprd_fraction[1];
                    if (!isZero(it_sd->spr_l_agr)) *it_sd_l = it_sd->amt_to_spr_l * sprd_fraction[2];
                    if (!isZero(it_sd->spr_m_agr)) *it_sd_m = it_sd->amt_to_spr_m * sprd_fraction[3];
                    if (!isZero(it_sd->spr_s_agr)) *it_sd_s = it_sd->amt_to_spr_s * sprd_fraction[4];
                    if (!isZero(it_sd->spr_e_agr)) *it_sd_e = it_sd->amt_to_spr_e * sprd_fraction[5];
                } else {
                    // use fraction calculated and placed at the [0] element
                    if (!isZero(sprd_fraction[0])) *it_sd_omh = it_sd->amt_to_spr_omh * sprd_fraction[0];
                    if (!isZero(sprd_fraction[0])) *it_sd_mh = it_sd->amt_to_spr_mh * sprd_fraction[0];
                    if (!isZero(sprd_fraction[0])) *it_sd_l = it_sd->amt_to_spr_l * sprd_fraction[0];
                    if (!isZero(sprd_fraction[0])) *it_sd_m = it_sd->amt_to_spr_m * sprd_fraction[0];
                    if (!isZero(sprd_fraction[0])) *it_sd_s = it_sd->amt_to_spr_s * sprd_fraction[0];
                    if (!isZero(sprd_fraction[0])) *it_sd_e = it_sd->amt_to_spr_e * sprd_fraction[0];
                }
            }
            it_sd++;
            it_tf++;
            it_sd_omh++;
            it_sd_mh++;
            it_sd_l++;
            it_sd_m++;
            it_sd_s++;
            it_sd_e++;
        }
    }
}


// 5.6
void recalc_spread_amounts_stepB(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd) {
    
    // Recalculate the Spread Aggregates by looping thru each Spread ID
    





}



// 5.7
void check_spread_amounts_totals(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd) {

    std::vector<final_results> chk_agr;
    for (auto& s : sd) {
        chk_agr.resize(sd.size(), { 0, "", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 } );
    }

    for (auto& up : upv) {
        //setup iterators to travere the respective categories within the 'up' vector item
        auto it_chk_agr = chk_agr.begin();

        auto it_sd_omh = up.sprd_omh.begin();
        auto it_sd_mh = up.sprd_mh.begin();
        auto it_sd_l = up.sprd_lab.begin();
        auto it_sd_m = up.sprd_mat.begin();
        auto it_sd_s = up.sprd_sub.begin();
        auto it_sd_e = up.sprd_eqp.begin();

        while ( it_chk_agr != chk_agr.end() &&
                it_sd_omh != up.sprd_omh.end() &&
                it_sd_mh != up.sprd_mh.end() &&
                it_sd_l != up.sprd_lab.end() &&
                it_sd_m != up.sprd_mat.end() &&
                it_sd_s != up.sprd_sub.end() &&
                it_sd_e != up.sprd_eqp.end() ) {

            it_chk_agr->omh += *it_sd_l;
            it_chk_agr->mh += *it_sd_l;
            it_chk_agr->area_qty  += up.area_qty;   // dont need iterator for qty since its unpivoted (i.e. only one per line)
            it_chk_agr->lab += *it_sd_l;
            it_chk_agr->mat += *it_sd_m;
            it_chk_agr->sub += *it_sd_s;
            it_chk_agr->eqp += *it_sd_e;

            it_chk_agr++;
            it_sd_omh++;
            it_sd_mh++;
            it_sd_l++;
            it_sd_m++;
            it_sd_s++;
            it_sd_e++;
        }
    }

    std::cout << "\ncheck data\n";
    for (auto& agr : chk_agr) {
    std::cout   
                << agr.omh << "|"
                << agr.mh << "|"
                << agr.lab << "|"
                << agr.mat << "|" 
                << agr.sub << "|" 
                << agr.eqp << "\n";
    }
}

// 5.8
void compute_sum_base_and_spread_amounts(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd, std::vector<final_results>& fd) {

    // Create a map to aggregate the unpivoted hours and cost data with Group by area and category
    std::map<int, std::map<std::string, final_results>> aggr_map_by_area_categ;

    for (auto& up : upv) {
        // Use the Map to gather the aggregates first for the base amounts
        aggr_map_by_area_categ[up.areaNo][up.brkd_ref].omh += up.omh;
        aggr_map_by_area_categ[up.areaNo][up.brkd_ref].mh += up.mh;
        if (starts_with_key(up.uom)){
            aggr_map_by_area_categ[up.areaNo][up.brkd_ref].area_qty += up.area_qty;
        }
        aggr_map_by_area_categ[up.areaNo][up.brkd_ref].lab += up.lab;
        aggr_map_by_area_categ[up.areaNo][up.brkd_ref].mat += up.mat;
        aggr_map_by_area_categ[up.areaNo][up.brkd_ref].sub += up.sub;
        aggr_map_by_area_categ[up.areaNo][up.brkd_ref].eqp += up.eqp;

        // Sum up all the spreads and add to the lab,mat,sub,eqp amounts above for each 'row'        
        auto it_somh = up.sprd_omh.begin();
        auto it_smh = up.sprd_mh.begin();
        // Do i need a spread for quantities ?????????????????????????????
        auto it_sl = up.sprd_lab.begin();
        auto it_sm = up.sprd_mat.begin();
        auto it_ss = up.sprd_sub.begin();
        auto it_se = up.sprd_eqp.begin();

        while ( it_somh != up.sprd_omh.end() && 
            it_smh != up.sprd_mh.end() && 
            it_sl != up.sprd_lab.end() && 
            it_sm != up.sprd_mat.end() && 
            it_ss != up.sprd_sub.end() && 
            it_se != up.sprd_eqp.end() ) {

            // Use the Map to gather the aggregates for thespread amounts using the iterator values
            aggr_map_by_area_categ[up.areaNo][up.brkd_ref].omh += *it_somh;
            aggr_map_by_area_categ[up.areaNo][up.brkd_ref].mh += *it_smh;
            // Do i need a spread for quantities ?????????????????????????????
            aggr_map_by_area_categ[up.areaNo][up.brkd_ref].lab += *it_sl;
            aggr_map_by_area_categ[up.areaNo][up.brkd_ref].mat += *it_sm;
            aggr_map_by_area_categ[up.areaNo][up.brkd_ref].sub += *it_ss;
            aggr_map_by_area_categ[up.areaNo][up.brkd_ref].eqp += *it_se;
            it_somh++;
            it_smh++;
            it_sl++;
            it_sm++;            
            it_ss++;
            it_se++;                        
        }
    }

    double tmp_total = 0;    
    for (auto& area_pair : aggr_map_by_area_categ) {
        for (auto& brkd_pair : area_pair.second) {
            tmp_total = brkd_pair.second.lab + brkd_pair.second.mat + brkd_pair.second.sub + brkd_pair.second.eqp;
            fd.push_back( { area_pair.first, brkd_pair.first, brkd_pair.second.omh, brkd_pair.second.mh, 
                            brkd_pair.second.area_qty, brkd_pair.second.lab, brkd_pair.second.mat, brkd_pair.second.sub, brkd_pair.second.eqp, tmp_total} );
        }
    }
} 

// 5.9
void create_aggregate_all_areas(std::vector<final_results>& fd) {

    // Create a map to aggregate the final results of all areas with Group by category only
    std::map<std::string, final_results> aggr_map_by_categ;

    for (auto& f : fd) {
        // Use the Map to gather the aggregates first for the base amounts
        aggr_map_by_categ[f.brkd_ref].omh += f.omh;
        aggr_map_by_categ[f.brkd_ref].mh+= f.mh;
        aggr_map_by_categ[f.brkd_ref].lab += f.lab;
        aggr_map_by_categ[f.brkd_ref].mat += f.mat;
        aggr_map_by_categ[f.brkd_ref].sub += f.sub;
        aggr_map_by_categ[f.brkd_ref].eqp += f.eqp;
    }

    // Print Headings
    std::cout   << "\nAggregte all areas\n"
                << "Category" << "|"
                << "Oth_MH" << "|"
                << "Mh_Tot" << "|"
                << "Qty" << "|"
                << "Lab" << "|"
                << "Mat" << "|"
                << "Sub" << "|"
                << "Eqp" << "\n";

    // Iterate through the map and print out the results

    auto it_agr = aggr_map_by_categ.begin();
    double tmp_total;

    while (it_agr !=  aggr_map_by_categ.end() ) {
        tmp_total = 0;

        std::cout   << it_agr->first << "|"
                    << it_agr->second.omh << "|"
                    << it_agr->second.mh << "|"
                    << it_agr->second.area_qty << "|"
                    << it_agr->second.lab << "|"
                    << it_agr->second.mat << "|"
                    << it_agr->second.sub << "|"
                    << it_agr->second.eqp << "|"
                    << "\n";
            tmp_total = it_agr->second.lab + it_agr->second.mat + it_agr->second.sub + it_agr->second.eqp;

            fd.push_back( { -1, 
                            it_agr->first, 
                            it_agr->second.omh,
                            it_agr->second.mh,
                            it_agr->second.area_qty,
                            it_agr->second.lab,
                            it_agr->second.mat,
                            it_agr->second.sub,
                            it_agr->second.eqp,
                            tmp_total
                        } );
        it_agr++;
    }

}


// 6. Print Results to Screen
// 6.1
void print_unpivot_data(std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd) {
    std::cout << "\nUnpivoted Data By Area:\n";
    std::cout   << std::setw( 6) << std::left << "areaNo" << "|" 
                << std::setw( 6) << "rowNo" << "|"
                << std::setw(42) << "brkd_ref" << "|"
                << std::setw(10) << "omh" << "|"
                << std::setw(10) << "area_qty" << "|"
                << std::setw(10) << "uom" << "|"
                << std::setw(10) << "mh" << "|"
                << std::setw(10) << "lab" << "|"
                << std::setw(10) << "mat" << "|"
                << std::setw(10) << "sub" << "|"
                << std::setw(10) << "eqp" << "|"
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
                    << std::setw(10) << up.omh << "|"
                    << std::setw(10) << up.area_qty << "|"
                    << std::setw(10) << up.uom << "|"
                    << std::setw(10) << up.mh << "|"
                    << std::setw(10) << up.lab << "|"
                    << std::setw(10) << up.mat << "|"
                    << std::setw(10) << up.sub << "|"
                    << std::setw(10) << up.eqp << "|"
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
        auto it_somh = up.sprd_omh.begin();
        auto it_smh = up.sprd_mh.begin();
        auto it_sl = up.sprd_lab.begin();
        auto it_sm = up.sprd_mat.begin();
        auto it_ss = up.sprd_sub.begin();
        auto it_se = up.sprd_eqp.begin();
        while ( 
                it_somh != up.sprd_omh.end() && 
                it_smh != up.sprd_mh.end() && 
                it_sl != up.sprd_lab.end() && 
                it_sm != up.sprd_mat.end() && 
                it_ss != up.sprd_sub.end() && 
                it_se != up.sprd_eqp.end() ) {

            std::cout << std::fixed << std::setprecision(2) << std::setw(10) << *it_somh << "|";
            std::cout << std::fixed << std::setprecision(2) << std::setw(10) << *it_smh << "|";
            std::cout << std::fixed << std::setprecision(2) << std::setw(10) << *it_sl << "|";
            std::cout << std::fixed << std::setprecision(2) << std::setw(10) << *it_sm << "|";
            std::cout << std::fixed << std::setprecision(2) << std::setw(10) << *it_ss << "|";
            std::cout << std::fixed << std::setprecision(2) << std::setw(10) << *it_se << "|";
            it_somh++;
            it_smh++;
            it_sl++;
            it_sm++;
            it_ss++;
            it_se++;
        }
    std::cout << "\n";
    }
}

// 6.2
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
void print_final_data(std::vector<final_results>& fd) {

    // Print Titles    
    std::cout << "\n";
    std::cout << std::setw(10) <<  std::left << "areaNo" << "|"
    << std::setw(30) << "brkd_ref" << "|" 
    << std::setw(10) << "omh" << "|" 
    << std::setw(10) << "mh" << "|" 
    << std::setw(10) << "area_qty" << "|" 
    << std::setw(10) << "lab" << "|"
    << std::setw(10) << "mat" << "|" 
    << std::setw(10) << "sub" << "|" 
    << std::setw(10) << "eqp" << "|" 
    << std::setw(10) << "total" << "\n"; 

    
    // Output to screen verify the read of the data
    int prevAreaNo = 0;
    for (const auto& f : fd) {

        if(f.areaNo != prevAreaNo ) {
            std::cout << "\n";
        }
        std::cout << std::setw(10) << std::left << f.areaNo << "|" 
            << std::setw(30) << f.brkd_ref << "|" 
            << std::setw(10) << f.omh << "|"
            << std::setw(10) << f.mh << "|"
            << std::setw(10) << f.area_qty << "|"
            << std::setw(10) << f.lab << "|"
            << std::setw(10) << f.mat << "|"
            << std::setw(10) << f.sub << "|"
            << std::setw(10) << f.eqp << "|"
            << std::setw(10) << f.total << "\n";
            prevAreaNo = f.areaNo;
    }
    std::cout << std::endl;
}

//7. Write Binary Output File for Excel Import
// 7.1
void open_file_to_write_results_binary(std::ofstream& outFile) {
    const std::string filename = "c:/t/brkd_output.bin";

    // Remove the file if it exists
    if (std::remove(filename.c_str()) == 0) {
        std::cout << "Previous file deleted." << std::endl;
    }

    outFile.open(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error opening file for writing!" << std::endl;
    }
}
// 7.2
void writeDbl(std::ofstream& file, double& num) {
    file.write(reinterpret_cast<const char*>(&num), sizeof(num));
}

// 7.3
void writeShr(std::ofstream& file, unsigned short& num) {
    file.write(reinterpret_cast<const char*>(&num), sizeof(num));
}

// 7.4
void writeInt(std::ofstream& file, int& num) {
    file.write(reinterpret_cast<const char*>(&num), sizeof(num));
}

// 7.5
void writeLong(std::ofstream& file, long& num) {
    file.write(reinterpret_cast<const char*>(&num), sizeof(num));
}

// 7.6
void writeStr(std::ofstream& file, std::string& str) {
    // write the string length as a single byte, 0-255 numeric
    unsigned char strL = str.length();
    file.write(reinterpret_cast<const char*>(&strL), sizeof(strL));
    // Write the string data
    file.write(str.data(), strL);
}

/*7.7*/
void write_unpivot_data(std::ofstream& outFile, std::vector<unpivot_area_data>& upv, std::vector<spread_items>& sd) {

    // Write the number of rows which cooresponds to vba integer type
    unsigned short row_cnt = upv.size();
    outFile.write(reinterpret_cast<const char*>(&row_cnt), sizeof(row_cnt));

    // Write the number of spreads which cooresponds to vba integer type
    unsigned short sprd_cnt = sd.size();
    outFile.write(reinterpret_cast<const char*>(&sprd_cnt), sizeof(sprd_cnt));

    //Write the Headers
    std::vector<std::string> titles = { "areaNo", "rowNo" ,"brkd_ref" ,"omh" ,"area_qty" ,"mh" ,"lab" ,"mat" ,"sub" ,"eqp" ,"total" ,"type" ,"div" ,"discp" ,"lab_type" ,"atl_btl" };
    for (auto& s : titles) {
        writeStr(outFile, s);
    }        

    int i=0;    
    std::string tmpStr;

    // write the Titles for Boolean values of condition whether to apply spreads as True of False
    for (const auto s : sd) {
        tmpStr = "Cond_" + std::to_string(i);
        writeStr(outFile, tmpStr);
        i++;
    }

    // Write Spread Computed Amounts Titles Spread Lab, Mat, Sub, Eqp
    i=0;
    for (auto s : sd) {
        tmpStr = "Spr_omh_" + std::to_string(i);
        writeStr(outFile, tmpStr);
        tmpStr = "Spr_mh_" + std::to_string(i);
        writeStr(outFile, tmpStr);
        tmpStr = "Spr_l_" + std::to_string(i);
        writeStr(outFile, tmpStr);
        tmpStr = "Spr_m_" + std::to_string(i);
        writeStr(outFile, tmpStr);
        tmpStr = "Spr_s_" + std::to_string(i);
        writeStr(outFile, tmpStr);
        tmpStr = "Spr_e_" + std::to_string(i);
        writeStr(outFile, tmpStr);
        i++;
    }

    // Write the Mix of Int, Strings, Dbl Values for each Row
    for (auto& up : upv) {
        writeInt(outFile, up.areaNo);
        writeInt(outFile, up.rowNo);
        writeStr(outFile, up.brkd_ref );
        writeDbl(outFile, up.omh);
        writeDbl(outFile, up.area_qty);
        writeDbl(outFile, up.mh);
        writeDbl(outFile, up.lab);
        writeDbl(outFile, up.mat);
        writeDbl(outFile, up.sub);
        writeDbl(outFile, up.eqp);
        writeDbl(outFile, up.total);
        writeStr(outFile, up.type);
        writeStr(outFile, up.div);
        writeStr(outFile, up.discp);
        writeStr(outFile, up.lab_type);
        writeStr(outFile, up.atl_btl);

        for (auto it_tf = up.apply_TF.begin(); it_tf < up.apply_TF.end(); it_tf++) {
            tmpStr = boolToString(*it_tf);
            writeStr(outFile, tmpStr);
        }
        // Next iterate thru the spread values and print
        auto it_somh = up.sprd_omh.begin();
        auto it_smh = up.sprd_mh.begin();
        auto it_sl = up.sprd_lab.begin();
        auto it_sm = up.sprd_mat.begin();
        auto it_ss = up.sprd_sub.begin();
        auto it_se = up.sprd_eqp.begin();
        while ( it_somh != up.sprd_omh.end() && 
                it_smh != up.sprd_mh.end() && 
                it_sl != up.sprd_lab.end() && 
                it_sm != up.sprd_mat.end() && 
                it_ss != up.sprd_sub.end() && 
                it_se != up.sprd_eqp.end() ) {
            writeDbl(outFile, *it_somh);
            writeDbl(outFile, *it_smh);
            writeDbl(outFile, *it_sl);
            writeDbl(outFile, *it_sm);
            writeDbl(outFile, *it_ss);
            writeDbl(outFile, *it_se);
            it_somh++;
            it_smh++;
            it_sl++;
            it_sm++;
            it_ss++;
            it_se++;
        }
    }
}
    
// 7.8
void write_sd(std::ofstream& outFile, std::vector<spread_items>& sd) {
    
    //Write the Headers
    std::vector<std::string> titles = { "sprd_order", "sprd_desc", "type", "div", "discp", "lab_type", "atl_btl", "sprd_basis_field", "incl_cumul_sprd_values_list",
                                        "amt_to_spr_omh", "amt_to_spr_mh", "amt_to_spr_l", "amt_to_spr_m", "amt_to_spr_s", "amt_to_spr_e",
                                        "spr_omh_agr", "spr_mh_agr", "spr_l_agr", "spr_m_agr", "spr_s_agr", "spr_e_agr" };
    for (auto& s : titles) {
        writeStr(outFile, s);
    }        

    // Output to screen verify the read of the data
    for (auto& s : sd) {
        writeShr(outFile, s.sprd_order);
        writeStr(outFile, s.sprd_desc);
        writeStr(outFile, s.type);
        writeStr(outFile, s.div);
        writeStr(outFile, s.discp);
        writeStr(outFile, s.lab_type);
        writeStr(outFile, s.atl_btl);
        writeStr(outFile, s.sprd_basis_field);
        writeStr(outFile, s.incl_cumul_sprd_values_list);

        writeDbl(outFile, s.amt_to_spr_omh);
        writeDbl(outFile, s.amt_to_spr_mh);
        writeDbl(outFile, s.amt_to_spr_l);
        writeDbl(outFile, s.amt_to_spr_m);
        writeDbl(outFile, s.amt_to_spr_s);
        writeDbl(outFile, s.amt_to_spr_e);

        writeDbl(outFile, s.spr_omh_agr);
        writeDbl(outFile, s.spr_mh_agr);
        writeDbl(outFile, s.spr_l_agr);
        writeDbl(outFile, s.spr_m_agr);
        writeDbl(outFile, s.spr_s_agr);
        writeDbl(outFile, s.spr_e_agr);
    }
}


// 7.9
void write_final_data(std::ofstream& outFile, std::vector<final_results>& fd) {

    //Write the number of fibal data rows
    unsigned short no_rows;
    no_rows =  static_cast<unsigned short>(fd.size());
    writeShr(outFile, no_rows);  // Write as a unsigned Short to be read as a integer in vba

    //Write the Headers
    std::vector<std::string> titles = { "areaNo", "brkd_ref", "omh", "mh", "area_qty", "lab", "mat", "sub", "eqp", "total" };
    for (auto& s : titles) {
        writeStr(outFile, s);
    }        

    // Output to screen verify the read of the data
    for (auto& f : fd) {
        writeInt(outFile, f.areaNo);
        writeStr(outFile, f.brkd_ref);
        writeDbl(outFile, f.omh);
        writeDbl(outFile, f.mh);
        writeDbl(outFile, f.area_qty);
        writeDbl(outFile, f.lab);
        writeDbl(outFile, f.mat);
        writeDbl(outFile, f.sub);
        writeDbl(outFile, f.eqp);
        writeDbl(outFile, f.total);
    }
    outFile.close();
    std::cout << "File closed after writing final data." << std::endl;
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


// 20.6
std::string boolToString(bool value) {
    return value ? "true" : "false";
}


// 20.7
bool starts_with_key(const std::string& str) {
    std::string prefix;
    prefix = "key_";

    return str.compare(0, prefix.size(), prefix) == 0;
}


// 20.8
bool is_str_match_w_wildcard(std::string& str1, std::string& str2) {
    if (str1 == str2) {
        return true;
    }
    if (str2 == "*") {
        return true;
    }
    return false;
}
