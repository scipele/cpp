#include <windows.h>
#include <bcrypt.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <array>
#include <filesystem>
#include "miniz.h"  // Include miniz for ZIP handling
#pragma comment(lib, "bcrypt.lib")

// Generic SHA-1 hashing function for raw data (used by both methods)
std::string hashDataWithSHA1(const std::vector<char>& data) {
    BCRYPT_ALG_HANDLE hAlgorithm;
    BCRYPT_HASH_HANDLE hHash;
    DWORD hashObjectSize, hashSize, bytesRead;
    std::vector<BYTE> hashObject, hashBuffer;

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

    // Hash the data
    if (BCryptHashData(hHash, (PUCHAR)data.data(), data.size(), 0) != 0) {
        throw std::runtime_error("Failed to hash data");
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

// Hash function for Office files (content-only, excluding customXml)
std::string hashOfficeContent(const std::string& filePath) {
    // Initialize miniz ZIP archive
    mz_zip_archive zip_archive = {0};
    if (!mz_zip_reader_init_file(&zip_archive, filePath.c_str(), 0)) {
        throw std::runtime_error("Failed to open ZIP archive: " + filePath);
    }

    BCRYPT_ALG_HANDLE hAlgorithm;
    BCRYPT_HASH_HANDLE hHash;
    DWORD hashObjectSize, hashSize, bytesRead;
    std::vector<BYTE> hashObject, hashBuffer;

    // Open the algorithm provider for SHA-1
    if (BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_SHA1_ALGORITHM, NULL, 0) != 0) {
        mz_zip_reader_end(&zip_archive);
        throw std::runtime_error("Failed to open SHA-1 algorithm provider");
    }

    // Get the object size for the hash
    if (BCryptGetProperty(hAlgorithm, BCRYPT_OBJECT_LENGTH, (PUCHAR)&hashObjectSize, sizeof(DWORD), &bytesRead, 0) != 0) {
        mz_zip_reader_end(&zip_archive);
        throw std::runtime_error("Failed to get hash object size");
    }
    hashObject.resize(hashObjectSize);

    // Get the hash length
    if (BCryptGetProperty(hAlgorithm, BCRYPT_HASH_LENGTH, (PUCHAR)&hashSize, sizeof(DWORD), &bytesRead, 0) != 0) {
        mz_zip_reader_end(&zip_archive);
        throw std::runtime_error("Failed to get hash length");
    }
    hashBuffer.resize(hashSize);

    // Create a hash object
    if (BCryptCreateHash(hAlgorithm, &hHash, hashObject.data(), hashObjectSize, NULL, 0, 0) != 0) {
        mz_zip_reader_end(&zip_archive);
        throw std::runtime_error("Failed to create hash");
    }

    // Iterate through all files in the ZIP archive
    int file_count = mz_zip_reader_get_num_files(&zip_archive);
    for (int i = 0; i < file_count; i++) {
        mz_zip_archive_file_stat file_stat;
        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) continue;

        std::string filename = file_stat.m_filename;
        // Skip the customXml folder (metadata)
        if (filename.find("customXml/") != std::string::npos) {
            std::cout << "Skipping: " << filename << " (metadata)" << std::endl;
            continue;
        }

        // Hash all other files
        size_t uncompressed_size = file_stat.m_uncomp_size;
        std::vector<char> buffer(uncompressed_size);
        if (mz_zip_reader_extract_to_mem(&zip_archive, i, buffer.data(), uncompressed_size, 0)) {
            std::cout << "Hashing: " << filename << " (" << uncompressed_size << " bytes)" << std::endl;
            if (BCryptHashData(hHash, (PUCHAR)buffer.data(), uncompressed_size, 0) != 0) {
                BCryptDestroyHash(hHash);
                BCryptCloseAlgorithmProvider(hAlgorithm, 0);
                mz_zip_reader_end(&zip_archive);
                throw std::runtime_error("Failed to hash data for " + filename);
            }
        }
    }

    // Finalize the hash
    if (BCryptFinishHash(hHash, hashBuffer.data(), hashSize, 0) != 0) {
        BCryptDestroyHash(hHash);
        BCryptCloseAlgorithmProvider(hAlgorithm, 0);
        mz_zip_reader_end(&zip_archive);
        throw std::runtime_error("Failed to finish hash");
    }

    // Clean up
    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlgorithm, 0);
    mz_zip_reader_end(&zip_archive);

    // Convert hash to hex string
    std::string hashString;
    for (BYTE byte : hashBuffer) {
        char hex[3];
        sprintf_s(hex, "%02x", byte);
        hashString += hex;
    }
    std::cout << "Final hash for " << filePath << ": " << hashString << std::endl;
    return hashString;
}

// Updated hashFileWithSHA1 to handle Office files
std::string hashFileWithSHA1(const std::string& filePath) {
    // Check if the file is an Office file based on extension
    std::filesystem::path path(filePath);
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // Office file extensions (add more as needed)
    std::vector<std::string> officeExtensions = {".xlsx", ".docx", ".pptx"};
    bool isOfficeFile = std::find(officeExtensions.begin(), officeExtensions.end(), ext) != officeExtensions.end();

    if (isOfficeFile) {
        return hashOfficeContent(filePath);
    }

    // For non-Office files, hash the entire file as before
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filePath);
    }

    // Read the entire file into memory (for simplicity; you can stream if needed)
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(fileSize);
    file.read(buffer.data(), fileSize);
    file.close();

    return hashDataWithSHA1(buffer);
}

int main() {
    std::array<std::string, 2> filePaths = {
        "C:/t/test/60-PKDS-Z3303_a.xlsx",
        "C:/t/test/60-PKDS-Z3303_b.xlsx"
    };

    // Hash each file
    for (const auto& file_path : filePaths) {
        try {
            std::string hash = hashFileWithSHA1(file_path);
            std::cout << "SHA-1 hash for " << file_path << ": " << hash << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    // Pause the console window before exiting
    system("pause");
    return 0;
}