//| Item	     | Class Documentation Notes                                   |
//|--------------|-------------------------------------------------------------|
//| Filename/type| CompareFiles.cpp / Class implmentation file                 |
//| EntryPoint   | instantiated from main                                      |
//| By Name,Date | T.Sciple, 8/30/2025                                         |

#include "../../include/CompareFiles.hpp"
#include <algorithm>
#include <cctype>


void CompareFiles::FindNewFiles() {
    std::cout << "\n\n";
    
    PrepForFileCopy();
    std::optional<std::ofstream> file_opt = getFileHandle();
    if (file_opt) {
        std::ofstream& file = *file_opt;  // Extract the std::ofstream reference

        writeHeaderLineToCsv(file);
        this->copied_count = 0;
        this->unique_files_count = 0;

        int id = 1;
        fileCateg categ;        
        // setup a loop to loop thru each potential scenario of FileType enums (0 , 1, 2)
        for (int i = 0; i < 3; i++) {
            
            int file_count;
            file_count = (i==0) ? newFiles.fileCount : origFiles.fileCount;
        
            std::string hash;
            for ( int j=0; j < file_count; j++) {

                // Determine the categ by i counter and hashes
                if (i==0) {
                    hash = newFiles.vecFileInfo[j].hash_code;
                    categ = (origFiles.hashes.count(hash)==0) ? in_new_no_duplicate : in_new_is_duplicate;
                } else {
                    hash = origFiles.vecFileInfo[j].hash_code;
                    categ = (newFiles.hashes.count(hash)==0) ? in_orig_no_duplicate : in_new_is_duplicate;
                }

                // Write the data in order of the 'i' loop and categ
                if (i == 0 && categ == in_new_no_duplicate) {
                    unique_files_count++;
                    copyOverNewFile(j);
                    writeDataLineToCsv(j, id, file, hash, categ);
                    id++;
                }

                // Write the data in order of the 'i' loop and categ
                if (i == 1 && categ == in_new_is_duplicate) {
                    writeDataLineToCsv(j, id, file, hash, categ);
                    id++;
                }

                // Write the data in order of the 'i' loop and categ
                if (i == 2 && categ == in_orig_no_duplicate) {
                    writeDataLineToCsv(j, id, file, hash, categ);
                    id++;
                }
            }
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


void CompareFiles::writeDataLineToCsv(int j, int id, std::ofstream& file, std::string& hash, fileCateg categ) { 
        
    int new_indx;
    // Get all the variables that are going to be output to the csv file
    // id   <- passed in function
    // hash <- passed in function
    // type <- passed in function
    int hash_count;
    std::string new_id; 
    std::string new_path;
    std::string new_filename;
    std::string orig_id; 
    std::string orig_path;
    std::string orig_file_name;

    switch (categ) {
        case in_new_no_duplicate:
            hash_count = 1;
            new_id = std::to_string(j+1);
            new_path = getStrFromConstWstr(newFiles.vecFileInfo[j].file_parent_path);
            new_filename = getStrFromConstWstr(newFiles.vecFileInfo[j].file_name);
            orig_id = "";
            orig_path = "";
            orig_file_name = "";
        break;

        case in_new_is_duplicate:
            hash_count = origFiles.vecFileInfo[j].hash_count;
            new_indx = newFiles.hashes[hash];
            new_id = std::to_string(new_indx+1);
            new_path = getStrFromConstWstr(newFiles.vecFileInfo[new_indx].file_parent_path);
            new_filename = getStrFromConstWstr(newFiles.vecFileInfo[new_indx].file_name);
            orig_id = std::to_string(j+1);
            orig_path = getStrFromConstWstr(origFiles.vecFileInfo[j].file_parent_path);
            orig_file_name = getStrFromConstWstr(origFiles.vecFileInfo[j].file_name);
        break;

        case in_orig_no_duplicate:
            hash_count = origFiles.vecFileInfo[j].hash_count;
            new_id = "";
            new_path = "";
            new_filename = "";
            orig_id = std::to_string(j+1);
            orig_path = getStrFromConstWstr(origFiles.vecFileInfo[j].file_parent_path);
            orig_file_name = getStrFromConstWstr(origFiles.vecFileInfo[j].file_name);
        break;
    }

    file << id << "|"
         << hash << "|"
         << hash_count << "|"
         << categ << "|"
         << new_id << "|"
         << new_path << "|"
         << new_filename << "|"
         << orig_id << "|"
         << orig_path << "|"
         << orig_file_name << "|"
         << std::endl;
}


int CompareFiles::PrepForFileCopy() {
    this->orig_path = origFiles.user_path;
    this->new_path = newFiles.user_path;
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
    file << "id|hash_code|hash_count|file_categ|new_id|new_path|new_file_name|orig_id|orig_path|orig_file_name" << std::endl;
}


void CompareFiles::copyOverNewFile(int i) {
    std::wstring source_path = getCopySourceFullPath(i);
    std::wstring dest_path_added = getCopyDestinationSeparateFullPath(i);

    // Ensure the destination directories exist
    std::filesystem::create_directories(std::filesystem::path(dest_path_added).parent_path());

    try {
        std::filesystem::copy(source_path, dest_path_added, std::filesystem::copy_options::overwrite_existing);
        this->copied_count++;
    } catch (const std::filesystem::filesystem_error& e) { }
}


std::wstring CompareFiles::getCopySourceFullPath(int i) {
    std::wstring fr_full_filename = 
        newFiles.vecFileInfo[i].file_parent_path +
        L"\\" +
        newFiles.vecFileInfo[i].file_name;
    return fr_full_filename;
}


// Clean the string by removing non-printable characters
void clean_string(std::wstring& str) {
    str.erase(std::remove_if(str.begin(), str.end(),
                             [](wchar_t c) { return c < 32 || c > 126; }),
              str.end());
}


// Find and replace old_dir
void replace_directory(std::wstring& dest_path) {
    // Clean the path
    clean_string(dest_path);

// Replace "2_latest_download" with "3_new_files" in the path
    const std::wstring old_dir = L"2_latest_download";
    const std::wstring new_dir = L"3_new_files";

    std::wstring search_str = L"\\" + old_dir;

    size_t pos = dest_path.find(search_str);
    if (pos != std::wstring::npos) {
        // Replace, skipping the leading backslash
        dest_path.replace(pos + 1, old_dir.length(), new_dir);
    } else {
        std::wcout << L"No match found.\n";
    }
}


std::wstring CompareFiles::getCopyDestinationSeparateFullPath(int i) {

    // Start with new_par_path, which contains "latest_download"
    std::wstring dest_path = newFiles.vecFileInfo[i].file_parent_path;

    replace_directory(dest_path);

    // Get filename, filename without extension, and extension
    std::wstring file_name = newFiles.vecFileInfo[i].file_name;
    std::wstring file_name_no_ext = stripExtension(file_name);
    std::wstring file_ext = file_name.substr(file_name_no_ext.size());

    // Construct the full destination path with date
    std::wstring dest_full_filename = 
        dest_path + L"\\" + 
        file_name_no_ext  + 
        // + L"_" + get_current_date() +    removed this
        file_ext;

    return dest_full_filename;
}

std::wstring CompareFiles::stripExtension(const std::wstring& filename) {
    size_t lastDot = filename.find_last_of('.');
    if (lastDot == std::wstring::npos) {
        return filename;  // No extension found
    }
    return filename.substr(0, lastDot);
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