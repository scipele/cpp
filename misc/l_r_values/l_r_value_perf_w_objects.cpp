#include <vector>
#include <chrono>
#include <iostream>

class BigObject {
public:
    std::vector<int> data;

    BigObject() : data(1'000'000, 42) { }   // 1 million ints ≈ 4 MB

    // Copy constructor (expensive)
    BigObject(const BigObject& other) : data(other.data) {
        //std::cout << "COPY constructed\n";   // uncomment to see calls
    }

    // Move constructor (fast)
    BigObject(BigObject&& other) noexcept : data(std::move(other.data)) {
        // std::cout << "MOVE constructed\n";   // uncomment to see calls
    }
};

int main() {
    const int iterations = 50;          // increase for bigger difference
    // const int inner_size = 1'000'000; // change inside BigObject if you want

    // ==================== COPY VERSION (lvalue) ====================
    auto start_copy = std::chrono::high_resolution_clock::now();

    std::vector<BigObject> container_copy;
    container_copy.reserve(iterations);   // prevents reallocations

    for (int i = 0; i < iterations; ++i) {
        BigObject temp;                    // named variable = lvalue
        container_copy.push_back(temp);    // calls COPY constructor
    }

    auto end_copy = std::chrono::high_resolution_clock::now();
    auto ms_copy = std::chrono::duration_cast<std::chrono::milliseconds>(end_copy - start_copy).count();

    // ==================== MOVE VERSION (rvalue) ====================
    auto start_move = std::chrono::high_resolution_clock::now();

    std::vector<BigObject> container_move;
    container_move.reserve(iterations);

    for (int i = 0; i < iterations; ++i) {
        BigObject temp;                         // still lvalue...
        container_move.push_back(std::move(temp)); // ...but we turn it into rvalue
    }

    auto end_move = std::chrono::high_resolution_clock::now();
    auto ms_move = std::chrono::duration_cast<std::chrono::milliseconds>(end_move - start_move).count();

    // ======================== RESULTS =========================
    std::cout << "=== Performance Comparison ===\n";
    std::cout << "Copy  (lvalue) : " << ms_copy << " ms  (" 
              << container_copy.size() << " objects)\n";
    std::cout << "Move  (rvalue) : " << ms_move << " ms  (" 
              << container_move.size() << " objects)\n";
    std::cout << "Speedup        : " << (double)ms_copy / ms_move << "× faster with move\n";

    // Uncomment the lines in the constructors above to see "COPY" vs "MOVE" printed
    return 0;
}