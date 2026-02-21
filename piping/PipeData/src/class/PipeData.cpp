// Filename:    PipeData.cpp
//
// Purpose:     This is the class implementation file that defines all the member functions of the class
//              1. The constructor creates an instance of the class and initializes the class member variables to blank/zero
//              2. Public Member Function/method 'void GetUserInput()' does the following
//                  2.1 Calls private member function 
//                      this->nom_size = get_str_from_user(msg...)
//                      returns size entered by user to the private member variable 'nom_size'
//
//                  2.2 Calls private member function again
//                      this->sched = get_str_from_user(msg...)
//                      returns size entered by user to the private member variable 'sched'
//
//                  2.3 Private member function is called which gets the OD and Thickness 
//                      int result = get_od_and_thk();
//                      the result is to capture any error messages
//                      Portions of two dimensional string array pipAry[][16] are read into map objects to find the matching Size and Schedule index
//                      The following private member variables are then set based on the index values returned by the map with size/sched
//                          this->pipeOD = std::stod(pipAry[size_index][1]);
//                          this->pipeThk = std::stod(pipAry[size_index][sch_index]);
//
//                  2.4 Private member function is called to print any errors if not = SUCCESS print_any_errors(result);
//
//                  2.5 Private member function 'get_addn_data()' is called which calculates and sets the following private class member variables
//                          this->pipeID 
//                          this->emptyWtPerFt
//                          this->waterWtPerFt
//                          this->pipeWtFullPerFt
//
//              3. Public member Funciton 'PrintPipeData()' Prints the values of the private member variables which are set by getter Functions / calculated    
//
//  By:         T.Sciple, 8/31/2024

#include "../../include/PipeData.hpp"
#include <iostream>
#include <cctype> // For std::toupper
#include <iomanip> // Include the header for setw()
#include <cmath>
#include <map>


// Define error codes
enum ErrorCode {
    SUCCESS = 0,
    ERROR_INVALID_SIZE = -1,
    ERROR_INVALID_SCHEDULE = -2
};

// ***********************  Base Class PipeData **************************
// This Constructor just instantiates the object and calls 'GetUserInput'
PipeData::PipeData() {
    GetUserInput();
}

// This is a destructor to assure that there will not be any memory leaks and class members are destructed 
PipeData::~PipeData() {
}


void PipeData::GetUserInput() {

    #ifdef _WIN32
        system("cls");
    #else
        std::cout << "\033[2J\033[H";
    #endif

    this->nom_size = get_str_from_user("1. Enter Nominal Pipe size in Decimal Format (0.5,1.25,2,3,6,8,12...) --> ");
    this->sched = get_str_from_user("2. Enter Pipe Schedule (std,xs,xxs,5,10,20,30,40,60,80,120,140,160) --> ");
    // Now get additional data using the user input
    int result = get_od_and_thk();
    // print any errors using the result integer return value from the 'get_od_and_thk' function
    print_any_errors(result);
    // calculation additional data
}


std::string PipeData::get_str_from_user(std::string user_msg) {
    std::string tmp;
    std::cout << user_msg;
    std::getline(std::cin, tmp);
    return PipeData::toUpperCase(tmp);
}


//function to convert string to uppercase
std::string PipeData::toUpperCase(const std::string str) {  
    std::string result = str;
    for (char& c : result) {
        c = std::toupper(c);
    }
    return result;
}


int PipeData::get_od_and_thk() {
    // Define Pipe Thickness Table from Federal_Steel_Supply_Pipe_Chart.pdf
    //              index j  ---->
    //  index   i   
    //          |
    //          v
    const std::string pipAry[][16] = {
       //0       ,1       ,2       ,3       ,4       ,5       ,6       ,7       ,8       ,9       ,10      ,11      ,12      ,13      ,14      ,15      },
        {"NPS"   ,"OD"    ,"5"     ,"10"    ,"20"    ,"30"    ,"40"    ,"STD"   ,"60"    ,"80"    ,"XS"    ,"100"   ,"120"   ,"140"   ,"160"   ,"XXS"   },		// 0       
        {"0.125" ,"0.405" ,".035"  ,".049"  ,"0"     ,"0"     ,".068"  ,".068"  ,"0"     ,".095"  ,".095"  ,"0"     ,"0"     ,"0"     ,"0"     ,"0"     },		// 1       
        {"0.25"  ,"0.54"  ,".049"  ,".065"  ,"0"     ,"0"     ,".088"  ,".088"  ,"0"     ,".119"  ,".119"  ,"0"     ,"0"     ,"0"     ,"0"     ,"0"     },		// 2       
        {"0.375" ,"0.675" ,".049"  ,".065"  ,"0"     ,"0"     ,".091"  ,".091"  ,"0"     ,".126"  ,".126"  ,"0"     ,"0"     ,"0"     ,"0"     ,"0"     },		// 3       
        {"0.5"   ,"0.84"  ,".065"  ,".083"  ,"0"     ,"0"     ,".109"  ,".109"  ,"0"     ,".147"  ,".147"  ,"0"     ,"0"     ,"0"     ,".187"  ,".294"  },		// 4       
        {"0.75"  ,"1.05"  ,".065"  ,".083"  ,"0"     ,"0"     ,".113"  ,".113"  ,"0"     ,".154"  ,".154"  ,"0"     ,"0"     ,"0"     ,".218"  ,".308"  },		// 5       
        {"1"     ,"1.315" ,".065"  ,".109"  ,"0"     ,"0"     ,".133"  ,".133"  ,"0"     ,".179"  ,".179"  ,"0"     ,"0"     ,"0"     ,".250"  ,".358"  },		// 6       
        {"1.25"  ,"1.66"  ,".065"  ,".109"  ,"0"     ,"0"     ,".140"  ,".140"  ,"0"     ,".191"  ,".191"  ,"0"     ,"0"     ,"0"     ,".250"  ,".382"  },		// 7       
        {"1.5"   ,"1.9"   ,".065"  ,".109"  ,"0"     ,"0"     ,".145"  ,".145"  ,"0"     ,".200"  ,".200"  ,"0"     ,"0"     ,"0"     ,".281"  ,".400"  },		// 8       
        {"2"     ,"2.375" ,".065"  ,".109"  ,"0"     ,"0"     ,".154"  ,".154"  ,"0"     ,".218"  ,".218"  ,"0"     ,"0"     ,"0"     ,".343"  ,".436"  },		// 9       
        {"2.5"   ,"2.875" ,".083"  ,".120"  ,"0"     ,"0"     ,".203"  ,".203"  ,"0"     ,".276"  ,".276"  ,"0"     ,"0"     ,"0"     ,".375"  ,".552"  },		// 10      
        {"3"     ,"3.5"   ,".083"  ,".120"  ,"0"     ,"0"     ,".216"  ,".216"  ,"0"     ,".300"  ,".300"  ,"0"     ,"0"     ,"0"     ,".437"  ,".600"  },		// 11      
        {"3.5"   ,"4"     ,".083"  ,".120"  ,"0"     ,"0"     ,".226"  ,".226"  ,"0"     ,".318"  ,".318"  ,"0"     ,"0"     ,"0"     ,"0"     ,".636"  },		// 12      
        {"4"     ,"4.5"   ,".083"  ,".120"  ,"0"     ,"0"     ,".237"  ,".237"  ,".281"  ,".337"  ,".337"  ,"0"     ,".437"  ,"0"     ,".531"  ,".674"  },		// 13      
        {"4.5"   ,"5"     ,"0"     ,"0"     ,"0"     ,"0"     ,"0"     ,".247"  ,"0"     ,"0"     ,".355"  ,"0"     ,"0"     ,"0"     ,"0"     ,".710"  },		// 14      
        {"5"     ,"5.563" ,".109"  ,".134"  ,"0"     ,"0"     ,".258"  ,".258"  ,"0"     ,".375"  ,".375"  ,"0"     ,".500"  ,"0"     ,".625"  ,".750"  },		// 15      
        {"6"     ,"6.625" ,".109"  ,".134"  ,"0"     ,"0"     ,".280"  ,".280"  ,"0"     ,".432"  ,".432"  ,"0"     ,".562"  ,"0"     ,".718"  ,".864"  },		// 16      
        {"7"     ,"7.625" ,"0"     ,"0"     ,"0"     ,"0"     ,"0"     ,".301"  ,"0"     ,"0"     ,".500"  ,"0"     ,"0"     ,"0"     ,"0"     ,".875"  },		// 17      
        {"8"     ,"8.625" ,".109"  ,".148"  ,".250"  ,".277"  ,".322"  ,".322"  ,".406"  ,".500"  ,".500"  ,".593"  ,".718"  ,".812"  ,".906"  ,".875"  },		// 18      
        {"9"     ,"9.625" ,"0"     ,"0"     ,"0"     ,"0"     ,"0"     ,".342"  ,"0"     ,"0"     ,".500"  ,"0"     ,"0"     ,"0"     ,"0"     ,"0"     },		// 19      
        {"10"    ,"10.75" ,".134"  ,".165"  ,".250"  ,".307"  ,".365"  ,".365"  ,".500"  ,".593"  ,".500"  ,".718"  ,".843"  ,"1.000" ,"1.125" ,"0"     },		// 20      
        {"11"    ,"11.75" ,"0"     ,"0"     ,"0"     ,"0"     ,"0"     ,".375"  ,"0"     ,"0"     ,".500"  ,"0"     ,"0"     ,"0"     ,"0"     ,"0"     },		// 21      
        {"12"    ,"12.75" ,".165"  ,".180"  ,".250"  ,".330"  ,".406"  ,".375"  ,".562"  ,".688"  ,".500"  ,".843"  ,"1.000" ,"1.125" ,"1.312" ,"0"     },		// 22      
        {"14"    ,"14"    ,"0"     ,".250"  ,".312"  ,".375"  ,".437"  ,".375"  ,".593"  ,".750"  ,".500"  ,".937"  ,"1.093" ,"1.250" ,"1.406" ,"0"     },		// 23      
        {"16"    ,"16"    ,"0"     ,".250"  ,".312"  ,".375"  ,".500"  ,".375"  ,".656"  ,".843"  ,".500"  ,"1.031" ,"1.218" ,"1.437" ,"1.593" ,"0"     },		// 24      
        {"18"    ,"18"    ,"0"     ,".250"  ,".312"  ,".437"  ,".562"  ,".375"  ,".750"  ,".937"  ,".500"  ,"1.156" ,"1.375" ,"1.562" ,"1.781" ,"0"     },		// 25      
        {"20"    ,"20"    ,"0"     ,".250"  ,".375"  ,".500"  ,".593"  ,".375"  ,".812"  ,"1.031" ,".500"  ,"1.280" ,"1.500" ,"1.750" ,"1.968" ,"0"     },		// 26      
        {"22"    ,"22"    ,"0"     ,".250"  ,".375"  ,".500"  ,"0"     ,".375"  ,".875"  ,"1.125" ,".500"  ,"1.375" ,"1.625" ,"1.875" ,"2.125" ,"0"     },		// 27      
        {"24"    ,"24"    ,"0"     ,".250"  ,".375"  ,".562"  ,".688"  ,".375"  ,".968"  ,"1.218" ,".500"  ,"1.531" ,"1.812" ,"2.062" ,"2.343" ,"0"     },		// 28      
        {"26"    ,"26"    ,"0"     ,".312"  ,".500"  ,"0"     ,"0"     ,".375"  ,"0"     ,"0"     ,".500"  ,".750"  ,"1.000" ,"1.250" ,"1.500" ,"2.000" },		// 29      
        {"28"    ,"28"    ,"0"     ,".312"  ,".500"  ,".625"  ,"0"     ,".375"  ,"0"     ,"0"     ,".500"  ,".750"  ,"1.000" ,"1.250" ,"1.500" ,"2.000" },		// 30      
        {"30"    ,"30"    ,"0"     ,".312"  ,".500"  ,".625"  ,"0"     ,".375"  ,"0"     ,"0"     ,".500"  ,".750"  ,"1.000" ,"1.250" ,"1.500" ,"2.000" },		// 31      
        {"32"    ,"32"    ,"0"     ,".312"  ,".500"  ,".625"  ,".688"  ,".375"  ,"0"     ,"0"     ,".500"  ,".750"  ,"1.000" ,"1.250" ,"1.500" ,"2.000" },		// 32      
        {"34"    ,"34"    ,"0"     ,".312"  ,".500"  ,".625"  ,".688"  ,".375"  ,"0"     ,"0"     ,".500"  ,".750"  ,"1.000" ,"1.250" ,"1.500" ,"2.000" },		// 33      
        {"36"    ,"36"    ,"0"     ,".312"  ,".500"  ,".625"  ,".750"  ,".375"  ,"0"     ,"0"     ,".500"  ,".875"  ,"1.000" ,"1.250" ,"1.500" ,"2.000" },		// 34      
        {"42"    ,"42"    ,"0"     ,"0"     ,"0"     ,"0"     ,"0"     ,".375"  ,"0"     ,"0"     ,".500"  ,".750"  ,"1.000" ,"1.250" ,"1.500" ,"2.000" },		// 35      
        {"48"    ,"48"    ,"0"     ,"0"     ,"0"     ,"0"     ,"0"     ,".375"  ,"0"     ,"0"     ,".500"  ,".750"  ,"1.000" ,"1.250" ,"1.500" ,"2.000" },		// 36      
    };	

    // Generte a map of sizes and check which one the user input
    int rows = sizeof(pipAry) / sizeof(pipAry[0]); // <- Rows calculate the size of the array dynamically
    std::map<std::string, int> sizeMap;  // define a map that will be used to return the size index

    for (int i=0; i<rows; i++) {    //  loop starts at 1 because first row is used for headings"
        sizeMap[pipAry[i][0]] = i;  // map is defined for the sizes
    }
    int size_index = sizeMap[this->nom_size];  // gets the index of the nominal size that the user input

    if(size_index == 0 ) {
        return ERROR_INVALID_SIZE;  // defined as -1
    }

    //generate a map of schedules that can be looked up verses user input
    std::map<std::string, int> schMap;
    // loop thru the pipe data array to find the matching schedule;
    int colms = sizeof(pipAry[0]) / sizeof(pipAry[0][0]); // Calculate the size of the row dynamically
    for (int j=1; j<colms; j++) {   //  loop starts at 1 because "NPS is not a schedule but is a reference title"
        schMap[pipAry[0][j]] = j;
    }
    int sch_index = schMap[this->sched];  // gets the index of the schedule that the user inputs

    if(sch_index == 0) {
        return ERROR_INVALID_SCHEDULE;  // defined as -2
    }

    this->pipeOD = std::stod(pipAry[size_index][1]);
    this->pipeThk = std::stod(pipAry[size_index][sch_index]);

    // Return a Success value defined as zero if no errors were encountered
    return SUCCESS;
 }


void PipeData::print_any_errors(int result) {
    if (result != SUCCESS) {
        // Handle errors
        switch (result) {
            case ERROR_INVALID_SIZE:
                std::cerr << "Error: No matching size was found.\n";
                break;
            case ERROR_INVALID_SCHEDULE:
                std::cerr << "Error: No matching schedule was found.\n";
                break;
            default:
                std::cerr << "Error: Unknown error occurred.\n";
        }
    }
}


void PipeData::PrintPipeData() {
    //Print data to screen
    std::cout << "\tEntered NPS =          " << std::setw(10) << std::left << this->nom_size << "\n";
    std::cout << "\tEntered Schedule =     " << std::setw(10) << std::left << this->sched << "\n";
    std::cout << "\tPipe OD =              " << this->pipeOD << "\n";
    std::cout << "\tPipe Thickness =       " << this->pipeThk << "\n";
}


// ***********************  Derived Class PipeDataExtended **************************
// Constructor for the extended properties calls the 'get_addn_data' function
PipeDataExtended::PipeDataExtended() {
    get_addn_data();
}

// Constructor for the extended properties calls the 'get_addn_data' function
PipeDataExtended::~PipeDataExtended() {
}


void PipeDataExtended::get_addn_data() {
    this->pipeID = this->pipeOD - 2 * this->pipeThk;

    //Calculate Empty Weight of Steel Pipe per Foot
    const double PI  = 3.14159265358979323846;
    this->emptyWtPerFt = PI / 4 * (pow(this->pipeOD, 2) - pow(this->pipeID, 2)) * 12 * 0.2836;

    //Calculate Empty Weight of Steel Pipe per Foot
    this->waterWtPerFt = PI / 4 * pow(this->pipeID /12, 2) * 1 * 62.4;
    
    //Calculate Full Weight Per Foot
    this->pipeWtFullPerFt = this->emptyWtPerFt + this->waterWtPerFt;
}


void PipeDataExtended::PrintPipeDataExt() {
    //Print data to screen
    std::cout << "Additional Data from PipeDataExtended Class:" << "\n";
    std::cout << "\tPipe ID =              " << this->pipeID << "\n";
    std::cout << "\tPipe Empty Wt/Ft =     " << this->emptyWtPerFt << "\n";
    std::cout << "\tWater #/Ft =           " << this->waterWtPerFt << "\n";
    std::cout << "\tPipe Full H2O #/Ft =   " << this->pipeWtFullPerFt << "\n\n";    
}