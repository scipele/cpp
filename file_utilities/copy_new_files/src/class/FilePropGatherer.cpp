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
                FileInfo { "" ,
                entry.path().parent_path(),
                entry.path().filename()} );
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