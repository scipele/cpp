#include <iostream>
#include <string>
#include <iomanip>
#include "std_lib/MurmurHash2.h"
#include "std_lib/MurmurHash3.h"


void print_hash(const std::string& hash_algo,
                std::string h) {
    
    std::cout   << std::setw(40) << std::left
                << hash_algo 
                << " = "
                << std::setw(22) << std::right
                << h
                << std::endl;
}


int main() {
    // Test String
    std::string str = "Hello World!";

    std::cout << "Hash values for string using various methods, str = '" << str << "'\n\n";
    
    // 1. Standard Hash
    std::hash<std::string> hasher;
    size_t std_hash = hasher(str);
    print_hash("Std Hash", std::to_string(std_hash));

    // 2. MurmurHash2
    // Use the same seed from functional_hash.h
    size_t seed = static_cast<size_t>(0xc70f6907UL);
    uint32_t murm2h = MurmurHash2(str.data(), str.length(), seed);
    print_hash("MurmurHash2", std::to_string(murm2h));

    // 3. uint64_t MurmurHash64A      ( const void * key, int len, uint64_t seed );
    uint64_t murm264a = MurmurHash64A(str.data(), str.length(), seed);
    print_hash("MurmurHash64A", std::to_string(murm264a));

    // 4. MurmurHash3 x64 128-bit
    // MurmurHash3_x64_128 ( const void * key, int len, uint32_t seed, void * out );
    uint64_t out[2];
    MurmurHash3_x64_128(str.data(), str.length(), seed, out);
    print_hash("MurmurHash3 x64 128-bit (lower 64 bits)", std::to_string(out[0]));
    print_hash("MurmurHash3 x64 128-bit (upper 64 bits)", std::to_string(out[1]));

    std::cout << std::endl;

    return 0;
}