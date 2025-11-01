#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include "ReadBom.hpp"

class WriteData {

public:
    //Declare a public method to write the file
    int WriteDataToCsv(std::vector<Bom>& bom) {
        int result  = OpenAndWriteFile(bom);
        return result;
    }

private:

    int OpenAndWriteFile(std::vector<Bom>& bom) {
        // Open a file for writing
        std::ofstream file("../output/results.csv");

        // Check if the file is open
        if (!file.is_open()) {
            std::cerr << "Failed to open the file.\n";
            return 1; // Exit with error code
        }
        // Write the header row (optional)
        file << "Index,Indx Code,Grp,Short Desc,Orig,Size1,Size2,Desc,Size1 Dec,Size2 Dec\n";
        // Write the data
        int i = 1;
        for (const auto& line : bom) {
            file << i << ","
                << line.indx_code << ","
                << line.grp << ","
                << line.short_desc << ","
                << line.orig << ","
                << line.size1 << ","
                << line.size2 << ","
                << line.desc << ","
                << line.size1_dec << ","
                << line.size2_dec << "\n";
            i++;
        }

        // Close the file
        file.close();

        std::cout << "Data written to results.csv\n";
        return 0;
    }
};
