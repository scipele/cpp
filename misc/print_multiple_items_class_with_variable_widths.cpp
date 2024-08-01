#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>

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
    // Constructor to set the default width and precision
    PrinterCls(int defaultWidth = 10, int precision = 2) : defaultWidth(defaultWidth), precision(precision) {}

    // Method to set the widths
    void set_widths(const std::vector<int>& newWidths) {
        widths = newWidths;
    }

    // Method to set the formats
    void set_format(const std::vector<char>& newFormats) {
        formats = newFormats;
    }

    // Method to set the alignments
    void set_alignment(const std::vector<char>& newAlignments) {
        alignments = newAlignments;
    }

    // Method to reset the widths, formats, and alignments to default
    void reset_width_to_default() {
        widths.clear();
        formats.clear();
        alignments.clear();
    }

    // Method to print items with the current widths, formats, and alignments
    template <typename... Args>
    void to_screen(const Args&... args) const {
        print_items(0, args...);
        std::cout << std::endl;
    }

private:
    int defaultWidth;
    int precision;
    std::vector<int> widths;
    std::vector<char> formats;
    std::vector<char> alignments;

    // Recursive variadic template function to handle multiple items
    template <typename T, typename... Args>
    void print_items(size_t index, const T& first, const Args&... args) const {
        int width = (index < widths.size()) ? widths[index] : defaultWidth;
        char format = (index < formats.size()) ? formats[index] : 's'; // Default to 's' (string format)
        char alignment = (index < alignments.size()) ? alignments[index] : 'l'; // Default to 'l' (left alignment)

        apply_format_and_alignment(first, format, alignment, width);
        std::cout << " | ";

        print_items(index + 1, args...);
    }

    // Base case to stop recursion
    void print_items(size_t) const {}

    // Apply the format and alignment to the given value
    void apply_format_and_alignment(const std::string& value, char format, char alignment, int width) const {
        if (alignment == 'c') {
            int padding = (width - value.length()) / 2;
            std::cout << std::setw(width) << std::setfill(' ') << std::right << value;
        } else if (alignment == 'r') {
            std::cout << std::setw(width) << std::setfill(' ') << std::right << value;
        } else {
            std::cout << std::setw(width) << std::setfill(' ') << std::left << value;
        }
    }

    void apply_format_and_alignment(int value, char format, char alignment, int width) const {
        if (alignment == 'c') {
            std::cout << std::setw(width) << std::setfill(' ') << std::right << value;
        } else if (alignment == 'r') {
            std::cout << std::setw(width) << std::setfill(' ') << std::right << value;
        } else {
            std::cout << std::setw(width) << std::setfill(' ') << std::left << value;
        }
    }

    void apply_format_and_alignment(double value, char format, char alignment, int width) const {
        std::ostringstream oss;

        if (format == 'a') {
            // Accounting format
            oss << std::fixed << std::setprecision(precision) << value;
            std::string str = oss.str();
            if (value < 0) {
                str = str.substr(1) + ")";
                str = "(" + str;
            }
            if (alignment == 'c') {
                int padding = (width - str.length()) / 2;
                std::cout << std::setw(width) << std::setfill(' ') << std::right << str;
            } else if (alignment == 'r') {
                std::cout << std::setw(width) << std::setfill(' ') << std::right << str;
            } else {
                std::cout << std::setw(width) << std::setfill(' ') << std::left << str;
            }
        } else {
            // Regular floating-point format
            oss << std::fixed << std::setprecision(precision) << value;
            std::string str = oss.str();
            if (alignment == 'c') {
                int padding = (width - str.length()) / 2;
                std::cout << std::setw(width) << std::setfill(' ') << std::right << str;
            } else if (alignment == 'r') {
                std::cout << std::setw(width) << std::setfill(' ') << std::right << str;
            } else {
                std::cout << std::setw(width) << std::setfill(' ') << std::left << str;
            }
        }
    }
};

int main() {
    // Create a Printer object with default width of 10
    PrinterCls prt;

    std::vector<estim_data> ed = {
        {"F-BW"   , 10, "EA", 565.34, 0, 0, 0},
        {"F-ER-SP", 50, "LF", 834.25, 302.5, 0, 0},
        {"F-ER-SP", 60, "LF", 1676.35, 302.5, 0, 0},
        {"F-ER-SP", 90, "LF", 1253, 0, 0, 0},
        {"F-ER-SP", 10, "LF", 5035.34, 302.5, 0, 0},
        {"FC&B"   , 10, "EA", 0, 0, 3450.65, 0}
    };

    // Example 1: Set custom widths, formats, and alignments for the columns and print to screen
    std::cout << "\n\nPrinting Example 1: Set custom widths, formats, and alignments for the columns and print to screen\n";
    prt.set_widths({ 15, 6, 6, 10, 10, 10, 10 });
    prt.set_alignment({ 'c', 'c', 'c', 'c', 'c', 'c', 'c' });  // Center all columns
    prt.to_screen("Desc", "Qty", "UOM", "Lab", "Matl", "Sub", "Eqp");

    prt.set_format({ 's', 'i', 's', 'a', 'a', 'a', 'a' });
    // Print data with custom widths, formats, and alignments
    for (const auto& e : ed) {
        prt.to_screen(e.desc, e.qty, e.uom, e.lab, e.mat, e.sub, e.eqp);
    }


    return 0;
}
