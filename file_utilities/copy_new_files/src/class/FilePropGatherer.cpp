#include "../../include/FilePropGatherer.hpp"


int FilePropGatherer::getFileProperties() {
    vecFileInfo.reserve(this->fileCount);
    const std::filesystem::path directory = user_path;
    int current_progress = 0;
    int progressStep = (fileCount > 0) ? fileCount / 100 : 1;  // Define step for progress updates

    std::cout << "\n";
    std::wcout << L"Getting file properties for " << this->user_path << "\n";

    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file() && !entry.is_symlink()) {

            vecFileInfo.emplace_back(
                FileInfo {
                    GetHashCode(entry.path().string()),
                    entry.path().parent_path(),
                    entry.path().filename()
                }
            );
            current_progress++;
            if (current_progress % progressStep == 0 || current_progress == this->fileCount) {
                prg.Update(current_progress);  // Update progress bar
            }
        }
    }
    return 0;
}


int FilePropGatherer::getFileCount() {
    size_t fl_count=0;
    const std::filesystem::path directory = user_path;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file() && !entry.is_symlink()) {
            fl_count ++;
        }
    }
    this->fileCount = fl_count;
    return 0;
}


int FilePropGatherer::getFolderCount() {
    int foldCount = 0;
    const std::filesystem::path directory = user_path;
    for (const auto & entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_directory()) {
            foldCount++;
        }
    }
    this->folderCount = foldCount;
    return 0;
}


std::string FilePropGatherer::get_current_date() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y_%m_%d");
    return ss.str();
}


std::string FilePropGatherer::GetHashCode(const std::string& filePath) {
    std::string hash;
    try {
        hash = hashFileWithSHA1(filePath);
    } catch (const std::exception& e) {
        //std::cerr << "Error: " << e.what() << std::endl;
        hash = "error in creating hash";  // indicate an error in the string result
    }
    return hash;
}


std::string FilePropGatherer::hashFileWithSHA1(const std::string& filePath) {
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


// Member function to output file properties to a CSV
void FilePropGatherer::OutputToCSV() {

    const std::wstring& filename = this->user_path + L"\\log.csv";
    // Open the file in write mode
    // Convert std::wstring to std::string (UTF-8 or ASCII)
    std::string filename_str(filename.begin(), filename.end());

    std::ofstream file(filename_str);
    
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    // Write header to the CSV
    file << "Hash Code,File Parent Path,File Name" << std::endl;

    // Write data for each file in vecFileInfo
    for (const auto& fileInfo : vecFileInfo) {
        file << fileInfo.hash_code << "|"
                << std::string(fileInfo.file_parent_path.begin(), fileInfo.file_parent_path.end()) << "|"
                << std::string(fileInfo.file_name.begin(), fileInfo.file_name.end()) << std::endl;
    }

    // Close the file
    file.close();
    std::cout << L"File properties written to " << filename_str << std::endl;
}