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
uint64_t MurmurHash64A_detailed ( const void * key, int len, uint64_t seed );
std::string int_to_bin(uint64_t value);
void out_pad(const std::string& label, auto&& value);


int main() {
    // Test String
    std::string str = "Mary";

    std::cout << "Hash values for string using various methods, str = '" << str << "'\n\n";
    
    // 1. Standard Hash
    std::hash<std::string> hasher;
    size_t std_hash = hasher(str);
    out_pad("1. Using std::hash<std::string>:", std::to_string(std_hash));

    // 2. MurmurHash2
    // Use the same seed from functional_hash.h
    size_t seed = static_cast<size_t>(0xc70f6907UL);
    uint32_t murm2h = MurmurHash2(str.data(), str.length(), seed);
    out_pad("2. MurmurHash2", std::to_string(murm2h));

    // 3. uint64_t MurmurHash64A      ( const void * key, int len, uint64_t seed );
    uint64_t murm264a = MurmurHash64A(str.data(), str.length(), seed);
    out_pad("3. MurmurHash64A", std::to_string(murm264a));

    // 4. MurmurHash3 x64 128-bit
    // MurmurHash3_x64_128 ( const void * key, int len, uint32_t seed, void * out );
    uint64_t out[2];
    MurmurHash3_x64_128(str.data(), str.length(), seed, out);
    out_pad("4.1 MurmurHash3x64 128-bit (lower bits)", std::to_string(out[0]));
    out_pad("4.2 MurmurHash3x64 128-bit (upper bits)", std::to_string(out[1]));

    std::cout << std::endl;

    // 5. Detailed MurmurHash2 computation
    MurmurHash64A_detailed(str.data(), str.length(), seed);

    return 0;
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
    std::cout << "Detailed MurmurHash64A computation steps for string '" << std::string((const char*)key, len) << "':\n";

    #define BIG_CONSTANT(x) (x)
    const uint64_t m = BIG_CONSTANT(0xc6a4a7935bd1e995);
    
    // Right-shift amount used in final avalanche mix steps
    const int r = 47;

    // Initialize hash value:
    // 1. Start with seed
    // 2. XOR with (length × multiplier) - this incorporates length into hash early
    uint64_t h = seed ^ (len * m);
    
    out_pad("seed (dec)" , seed);
    out_pad("len " , len);
    out_pad("m (const) " , m);
    out_pad("Initial hash value (h)", "seed ^ (len * m)");
    out_pad("Initial hash value (h)", 
            std::to_string(seed)
            + " xor (" + std::to_string(len)
            + " x " + std::to_string(m) + ") = "
            + std::to_string(h));
    out_pad("len * m (dec)" , len * m);
    out_pad("seed (bin)" , int_to_bin(seed));
    out_pad("len * m (bin)" , int_to_bin(len * m));
    out_pad("xor result h" , int_to_bin(h));

    const uint64_t * data = (const uint64_t *)key;
    const uint64_t * end = data + (len/8);

    int8_t step = 0;

    while(data != end)
    {
    std::cout << "\nProcessing 8-byte max block " << step << ":\n";
    out_pad("Original k ", *data);

    uint64_t k = *data++;
    out_pad("k *= m -> k", std::to_string(k) + " * " + std::to_string(m));

    k *= m; 
    out_pad("k (dec)", k);
    out_pad("k (bin)" , int_to_bin(k));
    out_pad("right shift by r=47 positions (k >> r)" , int_to_bin(k >> r)); 
    out_pad("now xor with orig k" , int_to_bin(k ^ (k >> r))); 

    k ^= k >> r; 
    out_pad("m (bin)" , int_to_bin(m)); 
    out_pad("now k is set to k * m" , int_to_bin(k * m)); 
    k *= m; 
    out_pad("now k is" , int_to_bin(k)); 
    out_pad("h is currently" , int_to_bin(h)); 
    out_pad("h is set to h xor k" , int_to_bin(h ^ k)); 
    h ^= k;
    out_pad("h is set to h * m" , int_to_bin(h * m)); 
    h *= m; 
    out_pad("h (bin)" , int_to_bin(h)); 
    step++; 
    }

    const unsigned char * data2 = (const unsigned char*)data;

    std::cout << "\nFinal Chunck Processing:" << (int)step << ":\n";
    switch(len & 7)
    {
    case 7: h ^= uint64_t(data2[6]) << 48;
            out_pad("remaining data2", int_to_bin(uint64_t(data2[6]))); 
            out_pad("right shift data by 48 positions ",int_to_bin(uint64_t(data2[6]) << 48)); 
            out_pad("h is set to h xor with above " , int_to_bin(h ^ (uint64_t(data2[6]) << 48))); 
    case 6: h ^= uint64_t(data2[5]) << 40;
            out_pad("remaining data2" , int_to_bin(uint64_t(data2[5]))); 
            out_pad("right shift data by 40 positions",int_to_bin(uint64_t(data2[5]) << 40));
            out_pad("h is set to h xor with above " , int_to_bin(h ^ (uint64_t(data2[5]) << 40)));
    case 5: h ^= uint64_t(data2[4]) << 32;
            out_pad("remaining data2" , int_to_bin(uint64_t(data2[4]))); 
            out_pad("right shift data by 32 positions",int_to_bin(uint64_t(data2[4]) << 32));
            out_pad("h is set to h xor with above " , int_to_bin(h ^ (uint64_t(data2[4]) << 32)));
    case 4: h ^= uint64_t(data2[3]) << 24;
            out_pad("remaining data2" , int_to_bin(uint64_t(data2[3]))); 
            out_pad("right shift data by 24 positions",int_to_bin(uint64_t(data2[3]) << 24));
            out_pad("h is set to h xor with above " , int_to_bin(h ^ (uint64_t(data2[3]) << 24)));
    case 3: h ^= uint64_t(data2[2]) << 16;
            out_pad("remaining data2" , int_to_bin(uint64_t(data2[2]))); 
            out_pad("right shift data by 16 positions", int_to_bin(uint64_t(data2[2]) << 16));
            out_pad("h is set to h xor with above " , int_to_bin(h ^ (uint64_t(data2[2]) << 16))); 
    case 2: h ^= uint64_t(data2[1]) << 8;
            out_pad("remaining data2" , int_to_bin(uint64_t(data2[1]))); 
            out_pad("right shift data by 8 positions ",int_to_bin(uint64_t(data2[1]) << 8)); 
            out_pad("h is set to h xor with above " , int_to_bin(h ^ (uint64_t(data2[1]) << 8))); 
    case 1: h ^= uint64_t(data2[0]);
            out_pad("remaining data2" , int_to_bin(uint64_t(data2[0]))); 
            out_pad("h is set to h xor with above " , int_to_bin(h ^ (uint64_t(data2[0]) << 40))); 
            out_pad("h (bin)" , int_to_bin(m)); 
            out_pad("now h is set to h * m" , int_to_bin(h * m)); 
            h *= m;
    };

    // Final mixing of the hash to ensure avalanche
    std::cout << "\nFinal mixing steps:\n";
    out_pad("h before final mix" ,  int_to_bin(h));
    out_pad("h >> r" , int_to_bin(h >> r));
    out_pad("h ^= h >> r" , int_to_bin(h ^ (h >> r)));
    h ^= h >> r;
    out_pad("h (bin)" , int_to_bin(h)); 
    out_pad("h *= m" , int_to_bin(h * m));
    h *= m;
    out_pad("h (bin)" , int_to_bin(h));
    out_pad("h >> r" ,  int_to_bin(h >> r));
    out_pad("h ^= h >> r" ,  int_to_bin(h ^ (h >> r)));
    h ^= h >> r;
    out_pad("Final hash value (h)" ,  int_to_bin(h));
    out_pad("Final hash value (h dec)" ,  h);
    // check against std::hash
    std::hash<std::string> hasher;
    size_t std_hash = hasher(static_cast<const char*>(key));
    out_pad("check vs std::hash (h dec)" , std_hash);
    std::string msg = (std_hash == h) ? "MATCHES" : "DOES NOT MATCH";
    out_pad("Result check: ", "calculated h " + msg + " std::hash");

    std::cout << std::endl;
    return h;
} 

void out_pad(const std::string& label, auto&& value) {
    std::cout << std::setw(40) << std::left << label << "= " << value << '\n';
}


std::string int_to_bin(uint64_t value) {
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