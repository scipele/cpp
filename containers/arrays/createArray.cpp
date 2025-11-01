#include <array>
#include <iostream>

// Hard-coded array dimensions
const int ROWS = 16;
const int COLS = 4;

std::array<std::array<int, COLS>, ROWS> create2DArray() {
    std::array<std::array<int, COLS>, ROWS> array;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            array[i][j] = i * COLS + j;
        }
    }
    return array;
}

int main() {
    auto arr = create2DArray();
    
    // Iterate and print each element
    int indx=0;
    std::string pad = "  ";
    for (const auto& rows : arr) {
        indx++;
        if(indx == 10) pad = " ";
        std::cout << "row= " << indx << ":" << pad;
        for (const auto& elem : rows) {
            std::cout << elem << " ";
        }
        std::cout << std::endl; // New line after each row
    }
    
    return 0;
}