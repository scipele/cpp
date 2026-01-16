#include <iostream>
#include <string>
#include <iomanip>
#include "std_lib/MurmurHash2.h"
#include "std_lib/MurmurHash3.h"


// Function prototypes
void print_hash(const std::string& hash_algo,std::string h);
uint64_t MurmurHash64A_detailed ( const void * key, int len, uint64_t seed );
std::string uInt64ToBinaryWithSpaces(uint64_t value);


int main() {
    // Test String
    std::string str = "Computer";

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

    // 5. Detailed MurmurHash2 computation
    MurmurHash64A_detailed(str.data(), str.length(), seed);

    return 0;
}


void print_hash(const std::string& hash_algo,
                std::string h) {
    
    std::cout   << std::setw(40) << std::left
                << hash_algo 
                << " = "
                << std::setw(22) << std::right
                << h
                << std::endl;
}



//-----------------------------------------------------------------------------
// MurmurHash2, 64-bit versions, by Austin Appleby

// The same caveats as 32-bit MurmurHash2 apply here - beware of alignment 
// and endian-ness issues if used across multiple platforms.

// 64-bit hash for 64-bit platforms

uint64_t MurmurHash64A_detailed ( const void * key, int len, uint64_t seed )
{
    // Detailed example of what is happening in MurmurHash2
    std::cout << "Detailed MurmurHash2 computation steps for string '" << std::string((const char*)key, len) << "':\n";

    #define BIG_CONSTANT(x) (x)
    const uint64_t m = BIG_CONSTANT(0xc6a4a7935bd1e995);
    
    // Right-shift amount used in final avalanche mix steps
    const int r = 47;

    // Initialize hash value:
    // 1. Start with seed
    // 2. XOR with (length × multiplier) - this incorporates length into hash early
    uint64_t h = seed ^ (len * m);
    
    std::cout << std::setw(40) << std::left << "seed (dec) " << "= " << seed << "\n";
    std::cout << std::setw(40) << std::left << "len " << "= " << len << "\n";
    std::cout << std::setw(40) << std::left << "m (const) " << "= " << m << "\n";
    std::cout << std::setw(40) << std::left << "Initial hash value (h)" << "= seed ^ (len * m)\n";
    std::cout << std::setw(40) << std::left << "Initial hash value (h)" << "=" << seed << " xor (" << len << " * " << m << ") = " << h << "\n";
    std::cout << std::setw(40) << std::left << "len * m (dec)" << "= " << len * m << "\n";
    std::cout << std::setw(40) << std::left << "seed (bin)" << "= " << uInt64ToBinaryWithSpaces(seed) << "\n";
    std::cout << std::setw(40) << std::left << "len * m (bin)" << "= " << uInt64ToBinaryWithSpaces(len * m) << "\n";
    std::cout << std::setw(40) << std::left << "xor result h" << "= " << uInt64ToBinaryWithSpaces(h) << "\n";

    const uint64_t * data = (const uint64_t *)key;
    const uint64_t * end = data + (len/8);

    int8_t step = 0;

    while(data != end)
    {
    std::cout << "\nProcessing 8-byte max block " << (int)step << ":\n";
    std::cout << std::setw(40) << std::left << "Original k " << "= " << *data << "\n";

    uint64_t k = *data++;
    std::cout << std::setw(40) << std::left << "Step 1: k *= m -> k = " << "= " << k << " * " << m << "\n";

    k *= m; 
    std::cout << std::setw(40) << std::left << "k (dec)" << "= " << k << "\n";
    std::cout << std::setw(40) << std::left << "k (bin)" << "= " << uInt64ToBinaryWithSpaces(k) << "\n";
    std::cout << std::setw(40) << std::left << "right shift by r=47 positions" << "= " << uInt64ToBinaryWithSpaces(k >> r) << "\n"; 

    k ^= k >> r; 
    k *= m; 

    h ^= k;
    h *= m; 
    step++; 
    }

    const unsigned char * data2 = (const unsigned char*)data;

    switch(len & 7)
    {
    case 7: h ^= uint64_t(data2[6]) << 48;
    case 6: h ^= uint64_t(data2[5]) << 40;
    case 5: h ^= uint64_t(data2[4]) << 32;
    case 4: h ^= uint64_t(data2[3]) << 24;
    case 3: h ^= uint64_t(data2[2]) << 16;
    case 2: h ^= uint64_t(data2[1]) << 8;
    case 1: h ^= uint64_t(data2[0]);
            h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    std::cout << std::setw(40) << std::left << "Final hash value (h)" << "= " << h << "\n";
    std::cout << std::endl;

    return h;

} 


std::string uInt64ToBinaryWithSpaces(uint64_t value) {
    std::string binaryString = "";
    int bitsToProcess = 64;

    while (bitsToProcess > 0) {
        // Process a byte (8 bits)
        for (int i = 7; i >= 0; --i) {
            // Check the i-th bit of the most significant byte to be processed
            if ((value >> (bitsToProcess - 1 - i)) & 1) {
                binaryString += '1';
            } else {
                binaryString += '0';
            }
        }
        bitsToProcess -= 8;
        
        // Add a space after each byte, unless it's the last one
        if (bitsToProcess > 0) {
            binaryString += ' ';
        }
    }

    return binaryString;
}