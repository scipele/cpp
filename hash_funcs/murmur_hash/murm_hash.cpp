// ************ MAIN PROGRAM ***************************************************
//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | murm_hash.cpp                                               |
//| EntryPoint   | main                                                        |
//| Purpose      | illustrate varius Hash algorithms and how they work         |
//| Inputs       | hard keyed                                                  |
//| Outputs      | screen print of results                                     |
//| Dependencies | Indicate if any libraries are used or none                  |
//| By Name,Date | T.Sciple, 01/16/2026                                        |


#include <iostream>
#include <string>
#include <iomanip>
#include "../std_lib/MurmurHash2.h"
#include "../std_lib/MurmurHash3.h"

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


// Added detailed MurmurHash2 computation with debug output
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
    std::cout << std::setw(40) << std::left << "right shift by r=47 positions (k >> r)" << "= " << uInt64ToBinaryWithSpaces(k >> r) << "\n"; 
    std::cout << std::setw(40) << std::left << "now xor with orig k" << "= " << uInt64ToBinaryWithSpaces(k ^ (k >> r)) << "\n"; 

    k ^= k >> r; 
    std::cout << std::setw(40) << std::left << "m (bin)" << "= " << uInt64ToBinaryWithSpaces(m) << "\n"; 
    std::cout << std::setw(40) << std::left << "now k is set to k * m" << "= " << uInt64ToBinaryWithSpaces(k * m) << "\n"; 
    k *= m; 
    std::cout << std::setw(40) << std::left << "now k is" << "= " << uInt64ToBinaryWithSpaces(k) << "\n"; 
    std::cout << std::setw(40) << std::left << "h is currently" << "= " << uInt64ToBinaryWithSpaces(h) << "\n"; 
    std::cout << std::setw(40) << std::left << "h is set to h xor k" << "= " << uInt64ToBinaryWithSpaces(h ^ k) << "\n"; 
    h ^= k;
    std::cout << std::setw(40) << std::left << "h is set to h * m" << "= " << uInt64ToBinaryWithSpaces(h * m) << "\n"; 
    h *= m; 
    std::cout << std::setw(40) << std::left << "h (bin)" << "= " << uInt64ToBinaryWithSpaces(h) << "\n"; 
    step++; 
    }

    const unsigned char * data2 = (const unsigned char*)data;

    std::cout << "\nFinal Chunck Processing:" << (int)step << ":\n";
    switch(len & 7)
    {
    case 7: h ^= uint64_t(data2[6]) << 48;
            std::cout << std::setw(40) << std::left << "remaining data2" << "= " << uInt64ToBinaryWithSpaces(uint64_t(data2[6])) << "\n"; 
            std::cout << std::setw(40) << std::left << "right shift data by 48 positions " << "= "  << uInt64ToBinaryWithSpaces(uint64_t(data2[6]) << 48) << "\n"; 
            std::cout << std::setw(40) << std::left << "h is set to h xor with above " << "= " << uInt64ToBinaryWithSpaces(h ^ (uint64_t(data2[6]) << 48)) << "\n"; 
    case 6: h ^= uint64_t(data2[5]) << 40;
            std::cout << std::setw(40) << std::left << "remaining data2" << "= " << uInt64ToBinaryWithSpaces(uint64_t(data2[5])) << "\n"; 
            std::cout << std::setw(40) << std::left << "right shift data by 40 positions " << "= "  << uInt64ToBinaryWithSpaces(uint64_t(data2[5]) << 40) << "\n"; 
            std::cout << std::setw(40) << std::left << "h is set to h xor with above " << "= " << uInt64ToBinaryWithSpaces(h ^ (uint64_t(data2[5]) << 40)) << "\n"; 
    case 5: h ^= uint64_t(data2[4]) << 32;
            std::cout << std::setw(40) << std::left << "remaining data2" << "= " << uInt64ToBinaryWithSpaces(uint64_t(data2[4])) << "\n"; 
            std::cout << std::setw(40) << std::left << "right shift data by 32 positions " << "= "  << uInt64ToBinaryWithSpaces(uint64_t(data2[4]) << 32) << "\n"; 
            std::cout << std::setw(40) << std::left << "h is set to h xor with above " << "= " << uInt64ToBinaryWithSpaces(h ^ (uint64_t(data2[4]) << 32)) << "\n"; 
    case 4: h ^= uint64_t(data2[3]) << 24;
            std::cout << std::setw(40) << std::left << "remaining data2" << "= " << uInt64ToBinaryWithSpaces(uint64_t(data2[3])) << "\n"; 
            std::cout << std::setw(40) << std::left << "right shift data by 24 positions " << "= "  << uInt64ToBinaryWithSpaces(uint64_t(data2[3]) << 24) << "\n"; 
            std::cout << std::setw(40) << std::left << "h is set to h xor with above " << "= " << uInt64ToBinaryWithSpaces(h ^ (uint64_t(data2[3]) << 24)) << "\n"; 
    case 3: h ^= uint64_t(data2[2]) << 16;
            std::cout << std::setw(40) << std::left << "remaining data2" << "= " << uInt64ToBinaryWithSpaces(uint64_t(data2[2])) << "\n"; 
            std::cout << std::setw(40) << std::left << "right shift data by 16 positions " <<  "= " << uInt64ToBinaryWithSpaces(uint64_t(data2[2]) << 16) << "\n"; 
            std::cout << std::setw(40) << std::left << "h is set to h xor with above " << "= " << uInt64ToBinaryWithSpaces(h ^ (uint64_t(data2[2]) << 16)) << "\n"; 
    case 2: h ^= uint64_t(data2[1]) << 8;
            std::cout << std::setw(40) << std::left << "remaining data2" << "= " << uInt64ToBinaryWithSpaces(uint64_t(data2[1])) << "\n"; 
            std::cout << std::setw(40) << std::left << "right shift data by 8 positions " << "= "  << uInt64ToBinaryWithSpaces(uint64_t(data2[1]) << 8) << "\n"; 
            std::cout << std::setw(40) << std::left << "h is set to h xor with above " << "= " << uInt64ToBinaryWithSpaces(h ^ (uint64_t(data2[1]) << 8)) << "\n"; 
    case 1: h ^= uint64_t(data2[0]);
            std::cout << std::setw(40) << std::left << "remaining data2" << "= " << uInt64ToBinaryWithSpaces(uint64_t(data2[0])) << "\n"; 
            std::cout << std::setw(40) << std::left << "h is set to h xor with above " << "= " << uInt64ToBinaryWithSpaces(h ^ (uint64_t(data2[0]) << 40)) << "\n"; 
            std::cout << std::setw(40) << std::left << "h (bin)" << "= " << uInt64ToBinaryWithSpaces(m) << "\n"; 
            std::cout << std::setw(40) << std::left << "now h is set to h * m" << "= " << uInt64ToBinaryWithSpaces(h * m) << "\n"; 
            h *= m;
    };

    // Final mixing of the hash to ensure avalanche
    std::cout << "\nFinal mixing steps:\n";
    std::cout << std::setw(40) << std::left << "h before final mix" << "= " <<  uInt64ToBinaryWithSpaces(h) << "\n";
    std::cout << std::setw(40) << std::left << "h >> r" << "= " << uInt64ToBinaryWithSpaces(h >> r) << "\n";
    std::cout << std::setw(40) << std::left << "h ^= h >> r" << "= " << uInt64ToBinaryWithSpaces(h ^ (h >> r)) << "\n";     
    h ^= h >> r;
    std::cout << std::setw(40) << std::left << "h (bin)" << "= " << uInt64ToBinaryWithSpaces(h) << "\n"; 
    std::cout << std::setw(40) << std::left << "h *= m" << "= " << uInt64ToBinaryWithSpaces(h * m) << "\n";
    h *= m;
    std::cout << std::setw(40) << std::left << "h (bin)" << "= " << uInt64ToBinaryWithSpaces(h) << "\n"; 
    std::cout << std::setw(40) << std::left << "h >> r" << "= " <<  uInt64ToBinaryWithSpaces(h >> r) << "\n";
    std::cout << std::setw(40) << std::left << "h ^= h >> r" << "= " <<  uInt64ToBinaryWithSpaces(h ^ (h >> r)) << "\n";
    h ^= h >> r;
    std::cout << std::setw(40) << std::left << "Final hash value (h)" << "= " <<  uInt64ToBinaryWithSpaces(h) << "\n";
    std::cout << std::setw(40) << std::left << "Final hash value (h dec)" << "= " <<  h << "\n";
    // check against std::hash
    std::hash<std::string> hasher;
    size_t std_hash = hasher(static_cast<const char*>(key));
    std::cout << std::setw(40) << std::left << "check vs std::hash (h dec)" << "= " << std_hash << "\n";
    std::string msg = (std_hash == h) ? "MATCHES" : "DOES NOT MATCH";
    std::cout << std::setw(40) << std::left << "Result check: " << "= calculated h " << msg + " std::hash\n";

    std::cout << std::endl;
    return h;
} 


std::string uInt64ToBinaryWithSpaces(uint64_t value) {
    std::string binaryString = "";
    
    // Iterate from bit 63 down to 0
    for (int i = 63; i >= 0; --i) {
        // Shift the specific bit we want to the 0-th position and mask it
        if ((value >> i) & 1) {
            binaryString += '1';
        } else {
            binaryString += '0';
        }
        // Add a space after every 8 bits, but not after the very last bit
        if (i > 0 && i % 8 == 0) {
            binaryString += ' ';
        }
    }
    return binaryString;
}