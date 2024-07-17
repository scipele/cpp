#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

struct sprds {
    std::string sprd_desc;
};

struct sprd_calc {
    double omh;
    double mh;
    double lab;
    double mat;
    double sub;
    double eqp;
};

struct estim_data {
    std::string desc;
    double omh;
    double mh;
    double lab;
    double mat;
    double sub;
    double eqp;
    std::vector<sprd_calc> sc;
};


int main() {
    std::vector<estim_data> ed;
    std::vector<sprds> sd;

    ed.push_back( { "bw 6 in", 0, 50, 100.0, 200.0, 300.0, 400.0 } );
    ed.push_back( { "bw 8 in", 0, 60, 110.0, 221.0, 310.0, 410.0 } );

    sd.push_back ( { "spread-dir" } );
    sd.push_back ( { "spread-cop" } );
    sd.push_back ( { "Per Diem" } );

    // Resize sc vector to hold 2 sprd_calc elements for each estim_data element
    for (auto& e : ed) {
        e.sc.resize(sd.size(), {0, 0, 0, 0, 0, 0});
    }

    for(auto& e : ed) {
        double i=1;
        for (auto& c : e.sc ) {
            c.omh = i * 0.05 * e.omh;
            c.mh = i * 0.05 * e.mh;
            c.lab = i * 0.05 * e.lab;
            c.mat = i * 0.05 * e.mat;
            c.sub = i * 0.05 * e.sub;
            c.eqp = i * .05 * e.eqp;
            i += 0.2;
        }
    }

    std::cout << "\n";
    std::cout << "\n";
    
    for(auto& e : ed) {
        std::cout << std::setw(15) << std::left << e.desc
        << std::setw(8) << e.mh
        << std::setw(8) << e.omh
        << std::setw(8) << e.lab
        << std::setw(8) << e.mat
        << std::setw(8) << e.sub
        << std::setw(8) << e.eqp;

        for (auto& c : e.sc) {
            std::cout << "|"
            << std::setw(8) << c.omh
            << std::setw(8) << c.mh
            << std::setw(8) << c.lab
            << std::setw(8) << c.mat
            << std::setw(8) << c.sub
            << std::setw(8) << c.eqp;
        }
        std::cout << "\n";
    }

    std::cout << "\n";
   // Pause the console window before exiting
    system("pause");

    return 0;
}