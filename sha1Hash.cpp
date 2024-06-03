#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>

std::string sha1File(const std::string& filePath) {
    // Open the file in binary mode
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }

    // Initialize variables
    uint32_t h0 = 0x67452301;
    uint32_t h1 = 0xEFCDAB89;
    uint32_t h2 = 0x98BADCFE;
    uint32_t h3 = 0x10325476;
    uint32_t h4 = 0xC3D2E1F0;

    // Process each 512-bit chunk
    while (!file.eof()) {
        char buffer[64];
        file.read(buffer, 64);

        // Pad the last chunk if necessary
        if (file.gcount() < 64) {
            buffer[file.gcount()] = 0x80;
            for (int i = file.gcount() + 1; i < 64; ++i) {
                buffer[i] = 0x00;
            }
            if (file.gcount() <= 56) {
                uint64_t ml = (file.tellg() - file.gcount()) * 8;
                for (int i = 63; i >= 56; --i) {
                    buffer[i] = (ml >> ((63 - i) * 8)) & 0xFF;
                }
            }
        }

        uint32_t w[80];
        for (int j = 0; j < 16; ++j) {
            w[j] = (buffer[j * 4] << 24) | (buffer[j * 4 + 1] << 16) | (buffer[j * 4 + 2] << 8) | (buffer[j * 4 + 3]);
        }
        for (int j = 16; j < 80; ++j) {
            w[j] = (w[j - 3] ^ w[j - 8] ^ w[j - 14] ^ w[j - 16]);
            w[j] = (w[j] << 1) | (w[j] >> 31); // Left-rotate by 1
        }

        uint32_t a = h0;
        uint32_t b = h1;
        uint32_t c = h2;
        uint32_t d = h3;
        uint32_t e = h4;

        // Main loop
        for (int j = 0; j < 80; ++j) {
            uint32_t f, k;
            if (j < 20) {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            }
            else if (j < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            }
            else if (j < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            }
            else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }

            uint32_t temp = ((a << 5) | (a >> 27)) + f + e + k + w[j];
            e = d;
            d = c;
            c = ((b << 30) | (b >> 2));
            b = a;
            a = temp;
        }

        // Update hash values
        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
    }

    // Concatenate hash components
    std::string hash;
    for (uint32_t i : {h0, h1, h2, h3, h4}) {
        for (int j = 24; j >= 0; j -= 8) {
            hash += static_cast<char>((i >> j) & 0xFF);
        }
    }

    return hash;
}

int main() {
    std::string filePath = "C:/Users/mscip/Desktop/Tony/Guitar/guitar music - Copy (2).xlsx";
    std::string hash = sha1File(filePath);
    if (!hash.empty()) {
        std::cout << "SHA-1 hash of file " << filePath << ": ";
        for (char c : hash) {
            printf("%02x", static_cast<unsigned char>(c));
        }
        std::cout << std::endl;
    }
    return 0;
}
