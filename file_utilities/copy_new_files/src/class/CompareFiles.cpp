//| Item	     | Class Documentation Notes                                   |
//|--------------|-------------------------------------------------------------|
//| Filename/type| CompareFiles.cpp / Class implmentation file                 |
//| EntryPoint   | instantiated from main                                      |
//| By Name,Date | T.Sciple, 1/19/2025                                         |

#include "../../include/CompareFiles.hpp"


void CompareFiles::FindNewFiles() {
    std::cout << "\n\n";
    
    PrepForFileCopy();
    std::optional<std::ofstream> file_opt = getFileHandle();
    if (file_opt) {
        std::ofstream& file = *file_opt;  // Extract the std::ofstream reference

        // Iterate thru the new files to see if they are actually new or already exist
        writeHeaderLineToCsv(file);
        this->copied_count = 0;
        this->unique_files_count = 0;
        for ( int i=0; i < newFiles.fileCount; i++) {
            // check if has exist in the original files and set 'isNew' flag
            std::string hash = newFiles.vecFileInfo[i].hash_code;
            bool isNew = (origFiles.hashes.count(hash) == 0) ? true : false;
            if (isNew) {
                unique_files_count++;
                copyOverNewFile(i);
            }
            writeDataLineToCsv(i, file, hash, isNew);
        }
        file.close();
        std::cout << "File properties written to: " 
                  << getStrFromConstWstr(newFiles.folder_parent_path)
                  << "\\log_combined.csv";
        } else {
            std::cerr << "Failed to open file." << std::endl;
        }
    std::cout << "\nNumber of files new copied: "
              << this->copied_count
              << " of "
              << this->unique_files_count
              << " unique files found ";
}


int CompareFiles::PrepForFileCopy() {
    this->orig_path = origFiles.user_path;
    this->new_path = newFiles.user_path;

    // Create a directory for added files with the current date
    std::wstring added_files_folder = 
        orig_path +
        L"/added_files_"
        + get_current_date();

    std::filesystem::create_directory(added_files_folder);
    return 0;
}


std::optional<std::ofstream> CompareFiles::getFileHandle() {
    std::wstring parent_pth = newFiles.folder_parent_path;
    std::wstring file_name = L"log_combined.csv";
    const std::wstring& full_name = parent_pth + L"\\" + file_name;
    std::string fullPathFileName_str = getStrFromConstWstr(full_name);

    std::ofstream file(fullPathFileName_str);
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return std::nullopt;  // Return no value
    }
    return file;
}

void CompareFiles::writeHeaderLineToCsv(std::ofstream& file) {
    file << "id|hash_code|is_new|new_path|new_file_name|dup_id|dup_path|dup_file_name|copy_to_path" << std::endl;
}


void CompareFiles::copyOverNewFile(int i) {
    std::string source_path = getCopySourceFullPath(i);
    std::string dest_path = getCopyDestinationFullPath(i);
    std::string dest_path_added = getCopyDestinationSeparateFullPath(i);

    /* used for testing only
    std::cout << "\nsrc: " << source_path << "\n"
              << "\t, dest: " << dest_path << "\n"
              << "\t, dest added: " << dest_path_added << "\n";*/

    // Ensure the destination directories exist
    std::filesystem::create_directories(std::filesystem::path(dest_path).parent_path());
    std::filesystem::create_directories(std::filesystem::path(dest_path_added).parent_path());

    try {
        std::filesystem::copy(source_path, dest_path, std::filesystem::copy_options::overwrite_existing);
        std::filesystem::copy(source_path, dest_path_added, std::filesystem::copy_options::overwrite_existing);
        this->copied_count++;
    } catch (const std::filesystem::filesystem_error& e) { }
}


std::string CompareFiles::getCopySourceFullPath(int i) {
    std::wstring fr_full_filename = 
        newFiles.vecFileInfo[i].file_parent_path +
        L"\\" +
        newFiles.vecFileInfo[i].file_name;
    return getStrFromWstr(fr_full_filename);
}


std::string CompareFiles::getCopyDestinationFullPath(int i) {
    
    std::wstring new_path = newFiles.vecFileInfo[i].file_parent_path;
    // strips specified number of chars from the left of the string
    std::wstring dest_path = this->orig_path + L"\\" + new_path.substr(this->new_path.size() + 1);

    std::wstring file_name = newFiles.vecFileInfo[i].file_name;
    std::wstring file_name_no_ext = stripExtension(newFiles.vecFileInfo[i].file_name);
    std::wstring file_ext = file_name.substr(file_name_no_ext.size());

    std::wstring dest_full_filename_added = 
        dest_path +
        L"\\" +
        file_name_no_ext +
        L"_" +
        get_current_date() +
        file_ext;

    return getStrFromWstr(dest_full_filename_added);
}


std::string CompareFiles::getCopyDestinationSeparateFullPath(int i) {

    std::wstring file_name = newFiles.vecFileInfo[i].file_name;
    std::wstring new_path = newFiles.vecFileInfo[i].file_parent_path;
    std::wstring dest_full_filename = 
        this->orig_path + 
        L"\\added_files_" +
        get_current_date() +
        L"\\" +
        file_name;

    return getStrFromWstr(dest_full_filename);
}

std::wstring CompareFiles::stripExtension(const std::wstring& filename) {
    size_t lastDot = filename.find_last_of('.');
    if (lastDot == std::wstring::npos) {
        return filename;  // No extension found
    }
    return filename.substr(0, lastDot);
}


void CompareFiles::writeDataLineToCsv(int i, std::ofstream& file, std::string& hash, bool isNew) { 
        
        int dup_file_indx = (isNew) ? -1 : origFiles.hashes[hash];
        file << i+1 << "|"
             << hash << "|" 
             << isNew << "|"
             << getStrFromConstWstr(newFiles.vecFileInfo[i].file_parent_path) << "|"
             << getStrFromConstWstr(newFiles.vecFileInfo[i].file_name) << "|";
        
        if (dup_file_indx == -1) {
            file << "-|-|-|"
                 << getCopyDestinationFullPath(i) << std::endl;
        } else {
            file << dup_file_indx + 1 << "|"
                 << getStrFromConstWstr(origFiles.vecFileInfo[dup_file_indx].file_parent_path) << "|"
                 << getStrFromConstWstr(origFiles.vecFileInfo[dup_file_indx].file_name) << "||"
                 << std::endl;
        }
}


std::string CompareFiles::getStrFromWstr(std::wstring& wstr) {
    // Using std::wstring_convert
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}


std::string CompareFiles::getStrFromConstWstr(const std::wstring& wstr) {
    // Using std::wstring_convert
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}


std::wstring CompareFiles::get_current_date() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::wstringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), L"%Y_%m_%d");
    return ss.str();
}