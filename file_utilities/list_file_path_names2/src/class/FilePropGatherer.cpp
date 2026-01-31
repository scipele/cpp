//| Item	     | Class Documentation Notes                                   |
//|--------------|-------------------------------------------------------------|
//| Filename/type| FilePropGatherer.cpp / Class implementation file            |
//| EntryPoint   | instantiated from main                                      |
//| By Name,Date | T.Sciple, 8/30/2025                                         |

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


// Member function to output file properties to a CSV
void FilePropGatherer::OutputToCSV(const std::wstring& file_name) {
    #ifdef _WIN32
        const std::wstring& filename = this->folder_parent_path + L"\\" + file_name;
    #else
        const std::wstring& filename = this->folder_parent_path + L"/" + file_name;
    #endif
    // Open the file in write mode
    // Convert std::wstring to std::string (UTF-8 or ASCII)
    std::string filename_str(filename.begin(), filename.end());

    std::ofstream file(filename_str);
    
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    // Write header to the CSV
    file << "id|File Parent Path|File Name" << std::endl;

    // Write data for each file in vecFileInfo
    size_t vecSize = vecFileInfo.size();
    for (size_t i = 0; i < vecSize; ++i) {
        file    << i+1 << "|"
                // the following converts wide string path to standard string
                << std::string(vecFileInfo[i].file_parent_path.begin(), vecFileInfo[i].file_parent_path.end()) << "|"
                << std::string(vecFileInfo[i].file_name.begin(), vecFileInfo[i].file_name.end()) << std::endl;
    }

    // Close the file
    file.close();
    
    std::cout << "\nFile properties written to " << filename_str;
}

