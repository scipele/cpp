#ifndef FILE_HASHER_H
#define FILE_HASHER_H

#include <string>
#include <vector>

class FileHasher {
public:
    // Explicit default constructor
    FileHasher();

    // Hash a file (Office or non-Office) and return SHA-1 hash as hex string
    std::string hashFile(const std::wstring& filePath);

private:
    // Initialize SHA-1 hash context
    void initSHA1Hash(void*& hAlgorithm, void*& hHash, std::vector<unsigned char>& hashObject, 
                      std::vector<unsigned char>& hashBuffer);

    // Update hash with data chunk
    void updateSHA1Hash(void* hHash, const char* data, size_t size);

    // Finalize SHA-1 hash and return hex string
    std::string finalizeSHA1Hash(void* hHash, void* hAlgorithm, std::vector<unsigned char>& hashBuffer);

    // Hash Office file content (excluding customXml)
    std::string hashOfficeContent(const std::wstring& filePath);

    // Hardcoded configuration
    static constexpr size_t bufferSize_ = 4096;
    static const std::vector<std::string> officeExtensions_;
};

#endif // FILE_HASHER_H