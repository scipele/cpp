#include <iostream>
#include <string>
#include <iomanip>
#include "std_lib/MurmurHash2.h"
#include "std_lib/MurmurHash3.h"


void print_hash(const std::string& s, 
                const std::string& hash_name,
                std::string h) {
    
    std::cout   << std::setw(40) << std::left
                << hash_name << " of '" + s + "'"
                << " = "
                << std::setw(22) << std::right
                << h
                << std::endl;
}


int main() {
    // Test String
    std::string str = "Mary";
    
    // 1. Standard Hash
    std::hash<std::string> hasher;
    size_t std_hash = hasher(str);
    print_hash(str, "Std Hash", std::to_string(std_hash));
    

    // starting seed used for various MurmurHash functions
    size_t seed = static_cast<size_t>(0xc70f6907UL);

    // 2. MurmurHash2
    // Use the same seed from functional_hash.h
    uint32_t murm2h = MurmurHash2(str.data(), str.length(), seed);
    print_hash(str, "MurmurHash2", std::to_string(murm2h));

    // 3. uint64_t MurmurHash64A      ( const void * key, int len, uint64_t seed );
    uint64_t murm264a = MurmurHash64A(str.data(), str.length(), seed);
    print_hash(str, "MurmurHash64A", std::to_string(murm264a));

    // 4. MurmurHash3 x64 128-bit
    // MurmurHash3_x64_128 ( const void * key, int len, uint32_t seed, void * out );
    uint64_t out[2];
    MurmurHash3_x64_128(str.data(), str.length(), seed, out);
    print_hash(str, "MurmurHash3 x64 128-bit (lower 64 bits)", std::to_string(out[0]));
    print_hash(str, "MurmurHash3 x64 128-bit (upper 64 bits)", std::to_string(out[1]));

    return 0;
}