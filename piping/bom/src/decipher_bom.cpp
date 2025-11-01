// filename:  decipher_bom.cpp test2
//
// Purpose:
//   1. Read bom description lines from a text file (each line as separate item)
//   2. Separate Size Information from the description into (sz_1, sz_2, desc) if requried
//   3. Categories each line into the following commodity coding system
//      AJ|XX|X|0001|AB|AB|11|00|A|X
//      |  |  |  |   |  |  |  |  | |
//      |  |  |  |   |  |  |  |  | |
//      |  |  |  |   |  |  |  |  | +---- end_typ_redc
//      |  |  |  |   |  |  |  |  +---- end_typ_main
//      |  |  |  |   |  |  |  +---- sch_redc
//      |  |  |  |   |  |  +---- sch_main
//      |  |  |  |   |  +---- matl_grade
//      |  |  |  |   +--- matl_code
//      |  |  |  +---- indx_code
//      |  |  +---- rtg
//      |  +---- size_redc
//      +---- size_main
//
//  4. Output file is generated that contains the following pipe ("|") delimited items
//      sz_1 | sz_2 | original_desc | standardized description | commodity code
//
//
// Dependencies:    Header Files            Class Name
//                  Timer.hpp               TimerCls
//                  ReadBom.hpp             ReadBom
//                  FracConvert.hpp         FracConvert
//                  WriteData.hpp           WriteData
//
// By:  T.Sciple, 8/14/2024

#include "../../../misc/Class/Timer.hpp"
#include "../include/ReadBom.hpp"
//#include "../include/FracConvert.hpp"
//#include "../include/CategorizeBom.hpp"
//#include "../include/WriteData.hpp"

int main(int argc, char const *argv[]) {
    //Start the Timer using the 'TimerCls' class called 'timer'
    TimerCls timer;
    timer.start();

    //*** Step 1 *** instantiate class as rb, then create vector of the bom
    ReadBom bm;
    bm.GetData();    // uses the .GetData Method in the class which reads the Bom
    timer.interval();

    //*** Step 2 *** Convert Sizes to Decimal using the FracConvert Class
    FracConvert conv;
    int result = conv.ConvertSizesToDec(bm);
    timer.interval();

/*
    //*** Step 3 *** Categorize the BOM Items
    CategorizeBom Ct;
    Ct.Read_Categ();
    Ct.LookupCategories(bm);
    timer.interval();

    //*** Step 4 *** Call the following class to Open and Write File
    WriteData wd;
    wd.WriteDataToCsv((bm));
    timer.interval();
*/

//Temp Print out that will list all BOM Items
        int i=1;
        for(auto& line : rb.bom_lines) {
            std::cout   << i << ". |"
                        << line.indx_code << "|"
                        << line.grp << "|"
                        << line.short_desc << "|"
                        << line.orig << "|"
                        << line.size1 << "|"
                        << line.size2 << "|"
                        << line.desc << "|"
                        << line.size1_dec << "|"
                        << line.size2_dec << "\n";
            i++;
        } 
    
    timer.end();
 
    system("pause");
    return 0;
}


