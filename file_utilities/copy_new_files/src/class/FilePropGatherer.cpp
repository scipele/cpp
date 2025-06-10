//| Item	     | Class Documentation Notes                                   |
//|--------------|-------------------------------------------------------------|
//| Filename/type| FilePropGatherer.cpp / Class implementation file            |
//| EntryPoint   | instantiated from main                                      |
//| By Name,Date | T.Sciple, 1/11/2025                                         |

#include "../../include/FilePropGatherer.hpp"

int FilePropGatherer::getFileProperties() {

    getFolderParentPath();

    vecFileInfo.reserve(this->fileCount);
    const std::filesystem::path directory = user_path;
    int current_progress = 0;
    // Define step for progress updates
    int progressStep = (fileCount > 500) ? fileCount / 100 : 1;  

    std::cout << "\n";
    std::wcout << L"Getting file properties for " << this->user_path << "\n";

    for (const auto& entry : 
        std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file() && !entry.is_symlink()) {

            vecFileInfo.emplace_back(
                FileInfo {
                    GetHashCode(entry.path().wstring()),
                    entry.path().parent_path().wstring(),
                    entry.path().filename().wstring()
                }
            );
            current_progress++;
            if (current_progress % progressStep == 0 ||
                current_progress == this->fileCount) {
                prg.Update(current_progress);  // Update progress bar
            }
        }
    }
    populate_map_with_hashes();
    return 0;
}


int FilePropGatherer::getFolderParentPath() {
    std::filesystem::path tmp_path(this->user_path);
    folder_parent_path = tmp_path.parent_path().wstring();
    return 0;
}


int FilePropGatherer::getFileCount() {
    size_t fl_count=0;
    const std::filesystem::path directory = user_path;
    for (const auto& entry :
        std::filesystem::recursive_directory_iterator(directory)) {
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
    for (const auto & entry :
        std::filesystem::recursive_directory_iterator(directory)) {
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


std::string FilePropGatherer::GetHashCode(const std::wstring& filePath) {
    std::string hash;
    try {
        hash = hashFileWithSHA1(filePath);
    } catch (const std::exception& e) {
        //std::cerr << "Error: " << e.what() << std::endl;
        hash = "error in creating hash at GetHashCode Member Func";  // indicate an error result
    }
    return hash;
}


std::string FilePropGatherer::hashFileWithSHA1(const std::wstring& filePath) {
    BCRYPT_ALG_HANDLE hAlgorithm;
    BCRYPT_HASH_HANDLE hHash;
    DWORD hashObjectSize, hashSize, bytesRead;
    std::vector<BYTE> hashObject, hashBuffer;
    std::ifstream file(filePath.c_str(), std::ios::binary);

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


int FilePropGatherer::populate_map_with_hashes() {

    // allocate space for the number of files
    hashes.reserve(fileCount);

    // First read in all the hashes and count duplicates
    size_t vecSize = vecFileInfo.size();
    for (size_t i = 0; i < vecSize; ++i) {
        std::string cur_hash = vecFileInfo[i].hash_code;
        if( hashes.count(cur_hash) > 0 ) {
            hashes[cur_hash]++;     // count duplicate files by incrementing
        } else {
            hashes[cur_hash] = 1;   // set count to 1 if no duplicates
        }
    }

    this->number_of_duplicate_files = 0;
    // Now count the number of duplicate files in the map
    for (auto pair : hashes) {
        if (pair.second > 1) {
            this->number_of_duplicate_files += pair.second - 1;
        }
    }
    std::cout << "Number of Duplicate Files: " << number_of_duplicate_files << "\n";


    // now that duplicate counts are known, write to fileInfo vector
    for (auto& cur_file : vecFileInfo ) {
        cur_file.hash_count = hashes[cur_file.hash_code];
    }


    // now that counts are set, now set the map second pair value to the 
    // vector index for reference later
    for (size_t i = 0; i < vecSize; ++i) {
        std::string cur_hash = vecFileInfo[i].hash_code;
        hashes[cur_hash] = i;  
    }
    return 0;
}


// Member function to output file properties to a CSV
void FilePropGatherer::OutputToCSV(const std::wstring& file_name) {

    const std::wstring& filename = this->folder_parent_path + L"\\" + file_name;
    // Open the file in write mode
    // Convert std::wstring to std::string (UTF-8 or ASCII)
    std::string filename_str(filename.begin(), filename.end());

    std::ofstream file(filename_str);
    
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    // Write header to the CSV
    file << "id|Hash Code|File Parent Path|File Name|hash_count" << std::endl;

    // Write data for each file in vecFileInfo
    size_t vecSize = vecFileInfo.size();
    for (size_t i = 0; i < vecSize; ++i) {
        file    << i+1 << "|"
                << vecFileInfo[i].hash_code << "|"
                // the following converts wide string path to standard string
                << std::string(vecFileInfo[i].file_parent_path.begin(), vecFileInfo[i].file_parent_path.end()) << "|"
                << std::string(vecFileInfo[i].file_name.begin(), vecFileInfo[i].file_name.end()) << "|"
                << vecFileInfo[i].hash_count << std::endl;
    }

    // Close the file
    file.close();
    
    std::cout << "\nFile properties written to " << filename_str;
}

