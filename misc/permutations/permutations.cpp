// ************ MAIN PROGRAM ***************************************************
//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | permutations.cpp                                            |
//| EntryPoint   | main                                                        |
//| Purpose      | determine permutations given parameters / constraints       |
//| Inputs       | hard coded const num_system_qty, no_positions               |
//| Outputs      | getPerms Funct provides vector of permutations by counting  |
//| Dependencies | none                                                        |
//| By Name,Date | T.Sciple, 7/20/2025                                         |

#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>

// Function declarations with consistent types
std::vector<std::vector<int>> getPerms(int rows,
                                       int cols,
                                       int num_system_qty);

void printPerms(const std::vector<std::vector<int>>& perms,
                int no_positions);

int main() {
    // Hard-coded parameters for the number system
    constexpr int num_system_qty = 2; // Base of the number system (e.g., 2 for binary)
    constexpr int no_positions = 4;   // Number of digits
    const int no_of_possibilities = std::pow(num_system_qty, no_positions);

    auto perms = getPerms(no_of_possibilities, no_positions, num_system_qty);
    printPerms(perms, no_positions);

    return 0;
}


// Generate permutations by converting row index to base num_system_qty
std::vector<std::vector<int>> getPerms(int rows,
                                       int cols,
                                       int num_system_qty) {
    
    // Validate inputs
    if (rows == 0 || cols == 0 || num_system_qty == 0) {
        return {};
    }

    // Initialize vector with zeros
    std::vector<std::vector<int>> perms(rows, std::vector<int>(cols, 0));

    // Generate permutations, storing digits in reverse order
    for (int i = 0; i < rows; ++i) {
        int value = i;
        for (int j = cols; j > 0; --j) {
            // Store least significant digit in perms[i][0]
            perms[i][j - 1] = value % num_system_qty; 
            value /= num_system_qty;
        }
    }

    return perms;
}


// Print permutations
void printPerms(const std::vector<std::vector<int>>& perms,
                int no_positions) {

    std::cout << "All possibilities:\n";
    for (int i = 0; i < perms.size(); ++i) {
        std::cout << std::setw(3) << std::right << i << ": ";
        for (int j = no_positions - 1; j >= 0; --j) {
            std::cout << std::setw(2) << std::right << perms[i][j] << " ";
        }
        std::cout << "\n";
    }
}