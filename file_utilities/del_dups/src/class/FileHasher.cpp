//| Item	     | Class Documentation Notes                                   |
//|--------------|-------------------------------------------------------------|
//| Filename/type| FileHasher.cpp / Class implmentation file                   |
//| EntryPoint   | instantiated from FilePropGatherer (FileHasher hasher;)     |
//| By Name,Date | T.Sciple, 8/30/2025      

#include <windows.h>
#include <bcrypt.h>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include "../../include/FileHasher.hpp"
#include "C:/dev_tools/vcpkg/installed/x64-windows-static/include/miniz/miniz.h"
#pragma comment(lib, "bcrypt.lib")


// Initialize static member
const std::vector<std::string> FileHasher::officeExtensions_ = {".xlsx", ".docx", ".pptx"};

// Explicit default constructor
FileHasher::FileHasher() {
    // No initialization needed; bufferSize_ and officeExtensions_ are static and initialized elsewhere
}

// Utility to convert std::wstring to std::string for miniz and file operations
std::string wstringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (size == 0) {
        throw std::runtime_error("Failed to convert wstring to string");
    }
    std::string str(size - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size, nullptr, nullptr);
    return str;
}


void FileHasher::initSHA1Hash(void*& hAlgorithm, void*& hHash, std::vector<unsigned char>& hashObject, 
                              std::vector<unsigned char>& hashBuffer) {
    DWORD hashObjectSize, hashSize, bytesRead;

    // Open the algorithm provider for SHA-1
    if (BCryptOpenAlgorithmProvider((BCRYPT_ALG_HANDLE*)&hAlgorithm, BCRYPT_SHA1_ALGORITHM, NULL, 0) != 0) {
        throw std::runtime_error("Failed to open SHA-1 algorithm provider");
    }

    // Get the object size for the hash
    if (BCryptGetProperty((BCRYPT_ALG_HANDLE)hAlgorithm, BCRYPT_OBJECT_LENGTH, (PUCHAR)&hashObjectSize, 
                          sizeof(DWORD), &bytesRead, 0) != 0) {
        throw std::runtime_error("Failed to get hash object size");
    }
    hashObject.resize(hashObjectSize);

    // Get the hash length
    if (BCryptGetProperty((BCRYPT_ALG_HANDLE)hAlgorithm, BCRYPT_HASH_LENGTH, (PUCHAR)&hashSize, 
                          sizeof(DWORD), &bytesRead, 0) != 0) {
        throw std::runtime_error("Failed to get hash length");
    }
    hashBuffer.resize(hashSize);

    // Create a hash object
    if (BCryptCreateHash((BCRYPT_ALG_HANDLE)hAlgorithm, (BCRYPT_HASH_HANDLE*)&hHash, hashObject.data(), 
                         hashObjectSize, NULL, 0, 0) != 0) {
        throw std::runtime_error("Failed to create hash");
    }
}


void FileHasher::updateSHA1Hash(void* hHash, const char* data, size_t size) {
    if (BCryptHashData((BCRYPT_HASH_HANDLE)hHash, (PUCHAR)data, size, 0) != 0) {
        throw std::runtime_error("Failed to hash data");
    }
}


std::string FileHasher::finalizeSHA1Hash(void* hHash, void* hAlgorithm, std::vector<unsigned char>& hashBuffer) {
    // Finalize the hash
    if (BCryptFinishHash((BCRYPT_HASH_HANDLE)hHash, hashBuffer.data(), hashBuffer.size(), 0) != 0) {
        throw std::runtime_error("Failed to finish hash");
    }

    // Convert hash to hex string
    std::string hashString;
    for (unsigned char byte : hashBuffer) {
        char hex[3];
        sprintf_s(hex, "%02x", byte);
        hashString += hex;
    }

    // Clean up
    BCryptDestroyHash((BCRYPT_HASH_HANDLE)hHash);
    BCryptCloseAlgorithmProvider((BCRYPT_ALG_HANDLE)hAlgorithm, 0);

    return hashString;
}


std::string FileHasher::hashOfficeContent(const std::wstring& filePath) {
    // Convert wstring to string for miniz
    std::string filePathStr = wstringToString(filePath);

    // Initialize miniz ZIP archive
    mz_zip_archive zip_archive = {0};
    if (!mz_zip_reader_init_file(&zip_archive, filePathStr.c_str(), 0)) {
        throw std::runtime_error("Failed to open ZIP archive: " + filePathStr);
    }

    void* hAlgorithm = nullptr;
    void* hHash = nullptr;
    std::vector<unsigned char> hashObject, hashBuffer;

    try {
        // Initialize hash context
        initSHA1Hash(hAlgorithm, hHash, hashObject, hashBuffer);

        // Iterate through all files in the ZIP archive
        int file_count = mz_zip_reader_get_num_files(&zip_archive);
        for (int i = 0; i < file_count; i++) {
            mz_zip_archive_file_stat file_stat;
            if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) continue;

            std::string filename = file_stat.m_filename;
            // Skip the customXml folder (metadata)
            if (filename.find("customXml/") != std::string::npos) {
                continue;
            }

            // Process ZIP entry in chunks
            size_t uncompressed_size = file_stat.m_uncomp_size;
            std::vector<char> buffer(bufferSize_);
            size_t total_read = 0;

            mz_zip_reader_extract_iter_state* state = mz_zip_reader_extract_iter_new(&zip_archive, i, 0);
            if (!state) {
                throw std::runtime_error("Failed to initialize ZIP extraction for " + filename);
            }

            while (total_read < uncompressed_size) {
                size_t bytes_to_read = std::min(bufferSize_, uncompressed_size - total_read);
                size_t bytes_read = mz_zip_reader_extract_iter_read(state, buffer.data(), bytes_to_read);
                if (bytes_read == 0 && total_read < uncompressed_size) {
                    mz_zip_reader_extract_iter_free(state);
                    throw std::runtime_error("Failed to read data for " + filename);
                }
                updateSHA1Hash(hHash, buffer.data(), bytes_read);
                total_read += bytes_read;
            }
            mz_zip_reader_extract_iter_free(state);
        }

        // Finalize and clean up
        std::string hashString = finalizeSHA1Hash(hHash, hAlgorithm, hashBuffer);
        mz_zip_reader_end(&zip_archive);
        return hashString;
    } catch (...) {
        if (hHash) BCryptDestroyHash((BCRYPT_HASH_HANDLE)hHash);
        if (hAlgorithm) BCryptCloseAlgorithmProvider((BCRYPT_ALG_HANDLE)hAlgorithm, 0);
        mz_zip_reader_end(&zip_archive);
        throw;
    }
}


std::string FileHasher::hashFile(const std::wstring& filePath) {
    // Check if the file is an Office file based on extension
    std::filesystem::path path(filePath);
    std::string ext = wstringToString(path.extension().wstring());
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    bool isOfficeFile = std::find(officeExtensions_.begin(), officeExtensions_.end(), ext) != officeExtensions_.end();

    if (isOfficeFile) {
        return hashOfficeContent(filePath);
    }

    // For non-Office files, hash the file in chunks
    std::string filePathStr = wstringToString(filePath);
    std::ifstream file(filePathStr, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filePathStr);
    }

    void* hAlgorithm = nullptr;
    void* hHash = nullptr;
    std::vector<unsigned char> hashObject, hashBuffer;

    try {
        // Initialize hash context
        initSHA1Hash(hAlgorithm, hHash, hashObject, hashBuffer);

        // Read and hash the file in chunks
        std::vector<char> buffer(bufferSize_);
        while (file.read(buffer.data(), buffer.size()) || file.gcount()) {
            updateSHA1Hash(hHash, buffer.data(), file.gcount());
        }
        file.close();

        // Finalize and return the hash
        return finalizeSHA1Hash(hHash, hAlgorithm, hashBuffer);
    } catch (...) {
        if (hHash) BCryptDestroyHash((BCRYPT_HASH_HANDLE)hHash);
        if (hAlgorithm) BCryptCloseAlgorithmProvider((BCRYPT_ALG_HANDLE)hAlgorithm, 0);
        if (file.is_open()) file.close();
        throw;
    }
}