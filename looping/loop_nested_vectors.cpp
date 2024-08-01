#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

struct sprds {
    std::string sprd_desc;
};

struct sprd_calc {
    double lab;
    double mat;
    double sub;
    double eqp;
};

struct estim_data {
    std::string desc;
    double lab;
    double mat;
    double sub;
    double eqp;
    std::vector<sprd_calc> sc;
};


int main() {
    std::vector<estim_data> ed;
    ed.push_back( { "bw 6 in", 100.0, 200.0, 300.0, 400.0 } );
    ed.push_back( { "bw 8 in", 110.0, 221.0, 310.0, 410.0 } );
    ed.push_back( { "bw 10 in", 120.0, 222.0, 320.0, 420.0 } );
    ed.push_back( { "bw 12 in", 130.0, 223.0, 330.0, 430.0 } );

    std::vector<sprds> sd;
    sd.push_back ( { "spread-dir" } );
    sd.push_back ( { "spread-cop" } );
    sd.push_back ( { "Per Diem" } );

    int num_sprds = sd.size();
    // outer loop auto iterator loops thru each 'ed' vector item
    for(auto& e : ed) {
        double i=1;
        e.sc.resize(num_sprds, {0, 0, 0, 0});
        // nested loop auto iterator loops thru each 'e.sc' vector item
        // in this example case there are three instances of the e.sc nested vector
        // within the sc vector since it is resized for each row to hold 'num_sprds' quantitie of elements for each estim_data element,
        for (auto& c : e.sc ) {
            c.lab = i * 0.05 * e.lab;
            c.mat = i * 0.05 * e.mat;
            c.sub = i * 0.05 * e.sub;
            c.eqp = i * .05 * e.eqp;
            i += 0.2;
        }
    }

    // Print elements of the Vector and the Nested Vector
    std::cout << "\n";
    std::cout << "Print all elements of the 'ed' vector and then nested 'sd' items preceded by |\n";
    
    for(auto& e : ed) {
        std::cout << std::setw(15) << std::left << e.desc
        << std::setw(6) << e.lab 
        << std::setw(6) << e.mat
        << std::setw(6) << e.sub
        << std::setw(6) << e.eqp;

        
        for (auto& c : e.sc) {
            std::cout << "|"
            << std::setw(6) << c.lab
            << std::setw(6) << c.mat
            << std::setw(6) << c.sub
            << std::setw(6) << c.eqp;
        }
        std::cout << "\n";
    }

    std::cout << "\n";
    std::cout << "Illustration of how to access the 3rd element of the main vector including all \n";
    // Test the ability to access elements by index say index [2] or 3rd element
        std::cout << std::setw(15) << std::left << ed[2].desc
        << std::setw(6) << ed[2].lab
        << std::setw(6) << ed[2].mat
        << std::setw(6) << ed[2].sub
        << std::setw(6) << ed[2].eqp;

            for (auto& c : ed[2].sc) {
            std::cout << "|"
            << std::setw(6) << c.lab
            << std::setw(6) << c.mat
            << std::setw(6) << c.sub
            << std::setw(6) << c.eqp;
        }


   // Pause the console window before exiting
    std::cout << "\n";
    system("pause");

    return 0;
}