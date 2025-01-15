#include <openssl/sha.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>

std::string computeSHA1(const std::string& data) {
    unsigned char hash[SHA_DIGEST_LENGTH]; // SHA-1 outputs 20 bytes
    SHA1(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);
    
    std::ostringstream oss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return oss.str();
}

int main() {
    std::string input = "Hello, World!";
    std::string hash = computeSHA1(input);
    std::cout << "SHA-1: " << hash << std::endl;
    return 0;
}
