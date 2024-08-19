// filename:  decipher_bom.cpp test
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

#include <vector>
#include "C:/dev/cpp/misc/Classes/Timer.hpp"
#include "ReadBom.hpp"
#include "FracConvert.hpp"
#include "CategorizeBom.hpp"
#include "WriteData.hpp"

int main(int argc, char const *argv[]) {
    //Start the Timer using the 'TimerCls' class called 'timer'
    TimerCls timer;
    timer.start();

    //*** Step 1 *** instantiate class as rb, then create vector of the bom
    ReadBom rb;
    std::vector<ReadBom::Bom> bom = rb.GetData();    // uses the .GetData Method in the class which reads the Bom
    timer.interval();

    //*** Step 2 *** Convert Sizes to Decimal using the FracConvert Class
    FracConvert conv;
    int result = conv.ConvertSizesToDec(bom);
    timer.interval();

    //*** Step 3 *** Categorize the BOM Items
    CategorizeBom Ct;
    Ct.Read_Categ();
    Ct.LookupCategories(bom);
    timer.interval();

    //*** Step 4 *** Call the following class to Open and Write File
    WriteData wd;
    wd.WriteDataToCsv(bom);
    timer.interval();
    timer.end();
 
    system("pause");
    return 0;
}


/*    //Temp Print out that will list all BOM Items
        int i=1;
        for(auto& line : bom) {
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
*/
