#include <windows.h>
#include <bcrypt.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <array>
#pragma comment(lib, "bcrypt.lib")


std::string hashFileWithSHA1(const std::string& filePath) {
    BCRYPT_ALG_HANDLE hAlgorithm;
    BCRYPT_HASH_HANDLE hHash;
    DWORD hashObjectSize, hashSize, bytesRead;
    std::vector<BYTE> hashObject, hashBuffer;
    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file");
    }

    // Open the algorithm provider for SHA-1
    if (BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_SHA1_ALGORITHM, NULL, 0) != 0) {
        throw std::runtime_error("Failed to open SHA-1 algorithm provider");
    }

    // Get the object size for the hash
    if (BCryptGetProperty(hAlgorithm, BCRYPT_OBJECT_LENGTH, (PUCHAR)&hashObjectSize, sizeof(DWORD), &bytesRead, 0) != 0) {
        throw std::runtime_error("Failed to get hash object size");
    }
    hashObject.resize(hashObjectSize);

    // Get the hash length
    if (BCryptGetProperty(hAlgorithm, BCRYPT_HASH_LENGTH, (PUCHAR)&hashSize, sizeof(DWORD), &bytesRead, 0) != 0) {
        throw std::runtime_error("Failed to get hash length");
    }
    hashBuffer.resize(hashSize);

    // Create a hash object
    if (BCryptCreateHash(hAlgorithm, &hHash, hashObject.data(), hashObjectSize, NULL, 0, 0) != 0) {
        throw std::runtime_error("Failed to create hash");
    }

    // Read the file and hash its contents
    char buffer[4096];
    while (file.read(buffer, sizeof(buffer)) || file.gcount()) {
        if (BCryptHashData(hHash, (PUCHAR)buffer, file.gcount(), 0) != 0) {
            throw std::runtime_error("Failed to hash data");
        }
    }

    // Finalize the hash
    if (BCryptFinishHash(hHash, hashBuffer.data(), hashSize, 0) != 0) {
        throw std::runtime_error("Failed to finish hash");
    }

    // Clean up
    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlgorithm, 0);

    // Convert hash to hex string
    std::string hashString;
    for (BYTE byte : hashBuffer) {
        char hex[3];
        sprintf_s(hex, "%02x", byte);
        hashString += hex;
    }
    return hashString;
}

int main() {

    std::array<std::string, 2> filePaths = {
        "C:/t/test/60-PKDS-Z3303_a.xlsx",
        "C:/t/test/60-PKDS-Z3303_b.xlsx"
    };

    // Method 2: Range-based for loop (C++11 and later)
    for (const auto& file_path : filePaths) {
        try {
            std::string hash = hashFileWithSHA1(file_path);
            std::cout << "SHA-1 hash a: " << hash << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }      
    }
    
    // 7. Pause the console window before exiting
    system("pause");
    return 0;
}
