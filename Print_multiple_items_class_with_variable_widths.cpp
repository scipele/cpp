#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

struct estim_data {
    std::string desc;
    int qty;
    std::string uom;
    double lab;
    double mat;
    double sub;
    double eqp;
};

// Define the Printer class
class PrinterCls {
public:
    // Constructor to set the default width
    PrinterCls(int defaultWidth = 10) : defaultWidth(defaultWidth) {}

    // Method to set the widths
    void setWidths(const std::vector<int>& newWidths) {
        widths = newWidths;
    }

    // Method to print items with the current widths
    template <typename... Args>
    void to_screen(const Args&... args) const {
        printItems(0, args...);
        std::cout << std::endl;
    }

private:
    int defaultWidth;
    std::vector<int> widths;

    // Recursive variadic template function to handle multiple items
    template <typename T, typename... Args>
    void printItems(size_t index, const T& first, const Args&... args) const {
        int width = (index < widths.size()) ? widths[index] : defaultWidth;
        std::cout << std::left << std::setw(width) << first << " | ";
        printItems(index + 1, args...);
    }

    // Base case to stop recursion
    void printItems(size_t) const {}
};


int main() {
    // Create Screen Print Object 
    PrinterCls prt;

    std::vector<estim_data> ed = {
        {"F-BW"    , 1, "EA", 565.34, 0, 0, 0 },
        {"F-ER-SP" , 3, "EA", 834.25, 302.5, 0, 0 },
        {"F-ER-SP" , 1, "EA", 1676.35, 302.5, 0, 0 },
        {"F-ER-SP" , 1, "LF", 1253, 0, 0, 0 },
        {"F-ER-SP" , 1, "LF", 5035.34, 302.5, 0, 0 },
        {"FC&B"    , 1, "EA", 0, 0, 3450.65, 0 }
    };
    
    // Print using default width
    std::cout << "\n\n\nExample printing using the print object with variable widths:\n";

    prt.setWidths( { 20,     8,     6,     10,    10,     10,    10 });
    prt.to_screen( "Desc", "Qty", "UOM", "Lab", "Matl", "Sub", "Eqp");
    for (auto& e : ed) {
        prt.to_screen(e.desc, e.qty, e.uom, e.lab, e.mat, e.sub, e.eqp);
    }

    return 0;
}
