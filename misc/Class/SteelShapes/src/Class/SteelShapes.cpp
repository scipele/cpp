// filename:    SteelShapes.cpp
// 
// Purpose:     SteelShapes Class Implementation File
//              - provides constructor and destructor prototypes
//              - provides function prototypes of member functions
// Dependencies:
//              - See SteelShapes.cpp for class implementation file
//
// By:  T. Sciple, 2024-10-14

#include "../../include/SteelShapes.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>


// This is the constructor which is basically similar to a function with the exact 
// same name as the class that automatically runs anytime that an instance of the
// Object 'SteelShapes' is created (aka instantiated).  

// Constructor set to NO_ERROR by default
SteelShapes::SteelShapes() : errorCode(ErrorCode::ERROR_NONE) {}

// Destructor to destroy the object at end of scope
SteelShapes::~SteelShapes() {
}

void SteelShapes::get_str_from_user() {
    for (int i=0; i<25; i++) std::cout << std::endl;
    std::string user_msg = "1. Enter Shape Desc and Size to get Properties (i.e. W14X109)-> ";
    std::cout << std::left << user_msg;
    std::string tmp;
    std::getline(std::cin, tmp);

    // set member variable to the value provided by the user
    this->input_name = SteelShapes::toUpperCase(tmp);



    int read_status = read_binary_file();

    if ( hasError() ) {
        std::cout << "Matching Shape Not Found\n";
        setError(ErrorCode::ERROR_BINARY_READ);
    } else {
        readCSV();
    }
}

//function to convert string to uppercase
std::string SteelShapes::toUpperCase(const std::string str) {  
    std::string result = str;
    for (char& c : result) {
        c = std::toupper(c);
    }
    return result;
}


int SteelShapes::read_binary_file() {
    std::ifstream file("../def_tables/shapes.bin", std::ios::binary);
    if (!file) {
        std::cerr << "Unable to open 'shapes.bin' file";
        return 1;
    }

    short row_cnt;
    
    // temp vector
    std::vector<std::string> temp[13];

    while (file.peek() != EOF) {       

        // Read the number of rows of data that were written in the Binary File
        file.read(reinterpret_cast<char*>(&row_cnt), sizeof(row_cnt));

        //Read each row from the Binary file
        std::string prev_type = "";
        int indx =-1;

        for (int i=0; i<row_cnt; i++) {
            // temporary printing of new type
            readString(file, Type);
            if (Type != prev_type) {
                indx++;
                // std::cout << "} \n\n" << Type << " {";
                prev_type = Type;
            }
            readString(file, EDI_Name);
            // temp
            //std::cout << "\"" << EDI_Name << "\",";
            temp[indx].push_back(EDI_Name);

            file.read(reinterpret_cast<char*>(&t_f), sizeof(t_f));
            file.read(reinterpret_cast<char*>(&W), sizeof(W));
            file.read(reinterpret_cast<char*>(&A), sizeof(A));
            file.read(reinterpret_cast<char*>(&d), sizeof(d));
            file.read(reinterpret_cast<char*>(&ddet), sizeof(ddet));
            file.read(reinterpret_cast<char*>(&Ht), sizeof(Ht));
            file.read(reinterpret_cast<char*>(&h), sizeof(h));
            file.read(reinterpret_cast<char*>(&OD), sizeof(OD));
            file.read(reinterpret_cast<char*>(&bf), sizeof(bf));
            file.read(reinterpret_cast<char*>(&bfdet), sizeof(bfdet));
            file.read(reinterpret_cast<char*>(&B1), sizeof(B1));
            file.read(reinterpret_cast<char*>(&b), sizeof(b));
            file.read(reinterpret_cast<char*>(&ID), sizeof(ID));
            file.read(reinterpret_cast<char*>(&tw), sizeof(tw));
            file.read(reinterpret_cast<char*>(&twdet), sizeof(twdet));
            file.read(reinterpret_cast<char*>(&twdet_2), sizeof(twdet_2));
            file.read(reinterpret_cast<char*>(&tf), sizeof(tf));
            file.read(reinterpret_cast<char*>(&tfdet), sizeof(tfdet));
            file.read(reinterpret_cast<char*>(&t), sizeof(t));
            file.read(reinterpret_cast<char*>(&tnom), sizeof(tnom));
            file.read(reinterpret_cast<char*>(&tdes), sizeof(tdes));
            file.read(reinterpret_cast<char*>(&kdes), sizeof(kdes));
            file.read(reinterpret_cast<char*>(&kdet), sizeof(kdet));
            file.read(reinterpret_cast<char*>(&k1), sizeof(k1));
            file.read(reinterpret_cast<char*>(&x), sizeof(x));
            file.read(reinterpret_cast<char*>(&y), sizeof(y));
            file.read(reinterpret_cast<char*>(&eo), sizeof(eo));
            file.read(reinterpret_cast<char*>(&xp), sizeof(xp));
            file.read(reinterpret_cast<char*>(&yp), sizeof(yp));
            file.read(reinterpret_cast<char*>(&bf_2tf), sizeof(bf_2tf));
            file.read(reinterpret_cast<char*>(&b_t), sizeof(b_t));
            file.read(reinterpret_cast<char*>(&b_tdes), sizeof(b_tdes));
            file.read(reinterpret_cast<char*>(&h_tw), sizeof(h_tw));
            file.read(reinterpret_cast<char*>(&h_tdes), sizeof(h_tdes));
            file.read(reinterpret_cast<char*>(&D_t), sizeof(D_t));
            file.read(reinterpret_cast<char*>(&Ix), sizeof(Ix));
            file.read(reinterpret_cast<char*>(&Zx), sizeof(Zx));
            file.read(reinterpret_cast<char*>(&Sx), sizeof(Sx));
            file.read(reinterpret_cast<char*>(&rx), sizeof(rx));
            file.read(reinterpret_cast<char*>(&Iy), sizeof(Iy));
            file.read(reinterpret_cast<char*>(&Zy), sizeof(Zy));
            file.read(reinterpret_cast<char*>(&Sy), sizeof(Sy));
            file.read(reinterpret_cast<char*>(&ry), sizeof(ry));
            file.read(reinterpret_cast<char*>(&Iz), sizeof(Iz));
            file.read(reinterpret_cast<char*>(&rz), sizeof(rz));
            file.read(reinterpret_cast<char*>(&Sz), sizeof(Sz));
            file.read(reinterpret_cast<char*>(&J), sizeof(J));
            file.read(reinterpret_cast<char*>(&Cw), sizeof(Cw));
            file.read(reinterpret_cast<char*>(&C), sizeof(C));
            file.read(reinterpret_cast<char*>(&Wno), sizeof(Wno));
            file.read(reinterpret_cast<char*>(&Sw1), sizeof(Sw1));
            file.read(reinterpret_cast<char*>(&Sw2), sizeof(Sw2));
            file.read(reinterpret_cast<char*>(&Sw3), sizeof(Sw3));
            file.read(reinterpret_cast<char*>(&Qf), sizeof(Qf));
            file.read(reinterpret_cast<char*>(&Qw), sizeof(Qw));
            file.read(reinterpret_cast<char*>(&ro), sizeof(ro));
            file.read(reinterpret_cast<char*>(&H1), sizeof(H1));
            file.read(reinterpret_cast<char*>(&tan_alpha), sizeof(tan_alpha));
            file.read(reinterpret_cast<char*>(&Iw), sizeof(Iw));
            file.read(reinterpret_cast<char*>(&zA), sizeof(zA));
            file.read(reinterpret_cast<char*>(&zB), sizeof(zB));
            file.read(reinterpret_cast<char*>(&zC), sizeof(zC));
            file.read(reinterpret_cast<char*>(&wA), sizeof(wA));
            file.read(reinterpret_cast<char*>(&wB), sizeof(wB));
            file.read(reinterpret_cast<char*>(&wC), sizeof(wC));
            file.read(reinterpret_cast<char*>(&SwA), sizeof(SwA));
            file.read(reinterpret_cast<char*>(&SwB), sizeof(SwB));
            file.read(reinterpret_cast<char*>(&SwC), sizeof(SwC));
            file.read(reinterpret_cast<char*>(&SzA), sizeof(SzA));
            file.read(reinterpret_cast<char*>(&SzB), sizeof(SzB));
            file.read(reinterpret_cast<char*>(&SzC), sizeof(SzC));
            file.read(reinterpret_cast<char*>(&rts), sizeof(rts));
            file.read(reinterpret_cast<char*>(&ho), sizeof(ho));
            file.read(reinterpret_cast<char*>(&PA), sizeof(PA));
            file.read(reinterpret_cast<char*>(&PA2), sizeof(PA2));
            file.read(reinterpret_cast<char*>(&PB), sizeof(PB));
            file.read(reinterpret_cast<char*>(&PC), sizeof(PC));
            file.read(reinterpret_cast<char*>(&PD), sizeof(PD));
            file.read(reinterpret_cast<char*>(&T1), sizeof(T1));
            file.read(reinterpret_cast<char*>(&WGi), sizeof(WGi));
            file.read(reinterpret_cast<char*>(&WGo), sizeof(WGo));

            if ( EDI_Name == input_name) {
                // exit the loop once a match is found for the input size
                file.close();
                return 0;
            }
        }
        int temp_int = TempWriteCSV(temp);    
    }
    file.close();
    // return -1 if not match is found
    return -1;
}


void SteelShapes::readCSV() {
    // This function reads a csv file that contains all the field names, unit of measures, etc.
    std::string filename = "../def_tables/field_names.csv";
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open 'field_names.csv' file: " << filename << std::endl;
        setError(ERROR_CSV_READ);
        return;
    }

    std::string line;
 
    // Read the file line by line
    while (std::getline(file, line)) {

        // Parse the line and store the struct in the vector
        field_def field = parseCSVLine(line);
        fields.push_back(field);
    }

    file.close();
    // testing-> std::cout << "csv fields read and closed\n";
}


// Helper function to split the CSV line into two fields
SteelShapes::field_def SteelShapes::parseCSVLine(const std::string& line) {
    field_def field;
    std::stringstream ss(line);
    std::getline(ss, field.field_name, ','); // Get the field name
    std::getline(ss, field.uom, ',');   // Get the variable name
    std::getline(ss, field.var_name, ',');        // Get the unit of measure (uom)

    return field;
}


int SteelShapes::PrintShapeProperties() {

    if ( hasError() ) {
        std::cout << "Error detected, cannot print shape properties\n";
        return -1;
    }

    // Prints the non zero properties of the shape input
    auto itt = fields.begin();
    std::cout               << std::left << std::setw(61) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << Type << " " << itt->uom << "\n"; itt++;
    std::cout               << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << EDI_Name << " " << itt->uom << "\n"; itt++;
    std::cout               << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << t_f << " " << itt->uom << "\n"; itt++;
    if(W>0) std::cout       << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << W << " " << itt->uom << "\n"; itt++;
    if(A>0) std::cout       << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << A << " " << itt->uom << "\n"; itt++;
    if(d>0) std::cout       << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << d << " " << itt->uom << "\n"; itt++;
    if(ddet>0) std::cout    << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << ddet << " " << itt->uom << "\n"; itt++;
    if(Ht>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << Ht << " " << itt->uom << "\n"; itt++;
    if(h>0) std::cout       << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << h << " " << itt->uom << "\n"; itt++;
    if(OD>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << OD << " " << itt->uom << "\n"; itt++;
    if(bf>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << bf << " " << itt->uom << "\n"; itt++;
    if(bfdet>0) std::cout   << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << bfdet << " " << itt->uom << "\n"; itt++;
    if(B1>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << B1 << " " << itt->uom << "\n"; itt++;
    if(b>0) std::cout       << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << b << " " << itt->uom << "\n"; itt++;
    if(ID>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << ID << " " << itt->uom << "\n"; itt++;
    if(tw>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << tw << " " << itt->uom << "\n"; itt++;
    if(twdet>0) std::cout   << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << twdet << " " << itt->uom << "\n"; itt++;
    if(twdet_2>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << twdet_2 << " " << itt->uom << "\n"; itt++;
    if(tf>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << tf << " " << itt->uom << "\n"; itt++;
    if(tfdet>0) std::cout   << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << tfdet << " " << itt->uom << "\n"; itt++;
    if(t>0) std::cout       << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << t << " " << itt->uom << "\n"; itt++;
    if(tnom>0) std::cout    << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << tnom << " " << itt->uom << "\n"; itt++;
    if(tdes>0) std::cout    << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << tdes << " " << itt->uom << "\n"; itt++;
    if(kdes>0) std::cout    << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << kdes << " " << itt->uom << "\n"; itt++;
    if(kdet>0) std::cout    << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << kdet << " " << itt->uom << "\n"; itt++;
    if(k1>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << k1 << " " << itt->uom << "\n"; itt++;
    if(x>0) std::cout       << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << x << " " << itt->uom << "\n"; itt++;
    if(y>0) std::cout       << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << y << " " << itt->uom << "\n"; itt++;
    if(eo>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << eo << " " << itt->uom << "\n"; itt++;
    if(xp>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << xp << " " << itt->uom << "\n"; itt++;
    if(yp>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << yp << " " << itt->uom << "\n"; itt++;
    if(bf_2tf>0) std::cout  << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << bf_2tf << " " << itt->uom << "\n"; itt++;
    if(b_t>0) std::cout     << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << b_t << " " << itt->uom << "\n"; itt++;
    if(b_tdes>0) std::cout  << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << b_tdes << " " << itt->uom << "\n"; itt++;
    if(h_tw>0) std::cout    << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << h_tw << " " << itt->uom << "\n"; itt++;
    if(h_tdes>0) std::cout  << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << h_tdes << " " << itt->uom << "\n"; itt++;
    if(D_t>0) std::cout     << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << D_t << " " << itt->uom << "\n"; itt++;
    if(Ix>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << Ix << " " << itt->uom << "\n"; itt++;
    if(Zx>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << Zx << " " << itt->uom << "\n"; itt++;
    if(Sx>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << Sx << " " << itt->uom << "\n"; itt++;
    if(rx>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << rx << " " << itt->uom << "\n"; itt++;
    if(Iy>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << Iy << " " << itt->uom << "\n"; itt++;
    if(Zy>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << Zy << " " << itt->uom << "\n"; itt++;
    if(Sy>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << Sy << " " << itt->uom << "\n"; itt++;
    if(ry>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << ry << " " << itt->uom << "\n"; itt++;
    if(Iz>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << Iz << " " << itt->uom << "\n"; itt++;
    if(rz>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << rz << " " << itt->uom << "\n"; itt++;
    if(Sz>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << Sz << " " << itt->uom << "\n"; itt++;
    if(J>0) std::cout       << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << J << " " << itt->uom << "\n"; itt++;
    if(Cw>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << Cw << " " << itt->uom << "\n"; itt++;
    if(C>0) std::cout       << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << C << " " << itt->uom << "\n"; itt++;
    if(Wno>0) std::cout     << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << Wno << " " << itt->uom << "\n"; itt++;
    if(Sw1>0) std::cout     << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << Sw1 << " " << itt->uom << "\n"; itt++;
    if(Sw2>0) std::cout     << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << Sw2 << " " << itt->uom << "\n"; itt++;
    if(Sw3>0) std::cout     << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << Sw3 << " " << itt->uom << "\n"; itt++;
    if(Qf>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << Qf << " " << itt->uom << "\n"; itt++;
    if(Qw>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << Qw << " " << itt->uom << "\n"; itt++;
    if(ro>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << ro << " " << itt->uom << "\n"; itt++;
    if(H1>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << H1 << " " << itt->uom << "\n"; itt++;
    if(tan_alpha>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << tan_alpha << " " << itt->uom << "\n"; itt++;
    if(Iw>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << Iw << " " << itt->uom << "\n"; itt++;
    if(zA>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << zA << " " << itt->uom << "\n"; itt++;
    if(zB>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << zB << " " << itt->uom << "\n"; itt++;
    if(zC>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << zC << " " << itt->uom << "\n"; itt++;
    if(wA>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << wA << " " << itt->uom << "\n"; itt++;
    if(wB>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << wB << " " << itt->uom << "\n"; itt++;
    if(wC>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << wC << " " << itt->uom << "\n"; itt++;
    if(SwA>0) std::cout     << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << SwA << " " << itt->uom << "\n"; itt++;
    if(SwB>0) std::cout     << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << SwB << " " << itt->uom << "\n"; itt++;
    if(SwC>0) std::cout     << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << SwC << " " << itt->uom << "\n"; itt++;
    if(SzA>0) std::cout     << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << SzA << " " << itt->uom << "\n"; itt++;
    if(SzB>0) std::cout     << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << SzB << " " << itt->uom << "\n"; itt++;
    if(SzC>0) std::cout     << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << SzC << " " << itt->uom << "\n"; itt++;
    if(rts>0) std::cout     << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << rts << " " << itt->uom << "\n"; itt++;
    if(ho>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << ho << " " << itt->uom << "\n"; itt++;
    if(PA>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << PA << " " << itt->uom << "\n"; itt++;
    if(PA2>0) std::cout     << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << PA2 << " " << itt->uom << "\n"; itt++;
    if(PB>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << PB << " " << itt->uom << "\n"; itt++;
    if(PC>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << PC << " " << itt->uom << "\n"; itt++;
    if(PD>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << PD << " " << itt->uom << "\n"; itt++;
    if(T1>0) std::cout      << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << T1 << " " << itt->uom << "\n"; itt++;
    if(WGi>0) std::cout     << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << WGi << " " << itt->uom << "\n"; itt++;
    if(WGo>0) std::cout     << std::left << std::setw(58) << std::setfill('.') << itt->field_name << " " << itt->var_name << " = " << WGo << " " << itt->uom << "\n";

    return 0;
}


void SteelShapes::readString(std::ifstream &file, std::string &str) {
      // Read length as a short, since written as an integer in vba
    short length;
    file.read(reinterpret_cast<char*>(&length), sizeof(length));
    str.resize(length);
    file.read(&str[0], length);
}


// Getter for errorCode
SteelShapes::ErrorCode SteelShapes::getErrorCode() const {
    return errorCode;
}

// Setter for errorCode
void SteelShapes::setError(ErrorCode code) {
    errorCode = code;
}


// Function to check if an error has occurred
bool SteelShapes::hasError() const {
    return errorCode != 0;  // Returns true if errorCode is non-zero
}


int SteelShapes::TempWriteCSV(std::vector<std::string> temp[13]) {
    // Define the CSV file name
    std::string filename = "output.csv";

    // Open an output file stream (ofstream) in write mode
    std::ofstream csvFile(filename);

    // Check if the file opened successfully
    if (!csvFile.is_open()) {
        std::cerr << "Could not open the file " << filename << " for writing!" << std::endl;
        return 1;
    }

        for (int i=0; i<13; i++) {
            for (auto rit = temp[i].rbegin() ; rit != temp[i].rend(); rit++ ) {
                csvFile << *rit << "\n";
            }
            csvFile << "NextType:\n";
        }

    // Close the file
    csvFile.close();

    std::cout << "Data written to " << filename << " successfully." << std::endl;

    return 0;
}