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
class PrintCls {
public:
    // Constructor to set the default width
    PrintCls(int defaultWidth = 10) : defaultWidth(defaultWidth) {}

    // Method to set the widths
    void set_widths(const std::vector<int>& newWidths) {
        widths = newWidths;
    }

    // Method to reset the widths to default
    void reset_wid_default() {
        widths.clear();
    }

    // Method to print items with the current widths
    template <typename... Args>
    void list(const Args&... args) const {
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
    // Create a Printer object with default width of 10
    PrintCls prt;

    std::vector<estim_data> ed = {
        {"F-BW"   , 10, "EA", 565.34, 0, 0, 0},
        {"F-ER-SP", 50, "LF", 834.25, 302.5, 0, 0},
        {"F-ER-SP", 60, "LF", 1676.35, 302.5, 0, 0},
        {"F-ER-SP", 90, "LF", 1253, 0, 0, 0},
        {"F-ER-SP", 10, "LF", 5035.34, 302.5, 0, 0},
        {"FC&B"   , 10, "EA", 0, 0, 3450.65, 0}
    };

    std::cout << "\n\nPrinting Example 1 Set custom widths for the columns and print to screen\n";
    prt.set_widths({ 15, 6, 6, 8, 8, 8, 8 });
    prt.list("Desc", "Qty", "UOM", "Lab", "Matl", "Sub", "Eqp");

    // Print data with custom widths
    for (const auto& e : ed) {
        prt.list(e.desc, e.qty, e.uom, e.lab, e.mat, e.sub, e.eqp);
    }

    std::cout << "\n\nPrinting Example 2 Set custom widths for the columns and print to screen\n";
    prt.reset_wid_default();
    prt.list("Desc", "Qty", "UOM", "Lab", "Matl", "Sub", "Eqp");
    // Print data with default width
    for (const auto& e : ed) {
        prt.list(e.desc, e.qty, e.uom, e.lab, e.mat, e.sub, e.eqp);
    }

    return 0;
}
