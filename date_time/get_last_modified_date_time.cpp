#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>

// Returns last modified time as a readable string (e.g., "2025-11-12 14:30:45")
std::string getLastModifiedReadable(const std::string& filePath) {
    auto lastWriteTime = std::filesystem::last_write_time(filePath);
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        lastWriteTime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
    );
    auto tt = std::chrono::system_clock::to_time_t(sctp);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&tt), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}


// Returns last modified time as a numeric timestamp (seconds since epoch)
//  Example: date/timestamp is converted to how local time is converted to an integer timestamp
//  Example Date/Time:  2025-11-26 21:10:49 UTC
//  Epoch Basis in c++	1/1/1970    time 0 hrs, 0 mins, 0 secs
//  +-------------------+-------+----------------+		
//  | uom               | qty   | seconds        |
//  +-------------------+-------+----------------+
//  | days delta        | 20418 | 1,764,115,200  |
//  | hrs delta	        | 21    |        75,600  |
//  | mins delta        | 10	|           600  |
//  | secs delta        | 49	|            49  |
//  | Local offset hrs  | UTC+6 |        21,600  |
//  +-------------------+-------+----------------+ 		                  
//                        Total   1,764,213,049  
std::int64_t getLastModifiedNumericLocal(const std::string& filePath) {
    auto lastWriteTime = std::filesystem::last_write_time(filePath);
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        lastWriteTime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
    );
    auto tt = std::chrono::system_clock::to_time_t(sctp);
    return static_cast<std::int64_t>(tt);
}


// Returns last modified time as a raw numeric timestamp (underlying count since epoch)
//  Example: date/timestamp is converted to how file_time_type internally represents time points    
auto getLastModifiedNumericRaw(const std::string& filePath) {
    auto lastWriteTime = std::filesystem::last_write_time(filePath);
    return static_cast<std::filesystem::file_time_type::duration::rep>(lastWriteTime.time_since_epoch().count());
}


void writeFileTimes(std::time_t timestampA, std::time_t timestampB) {
    std::ofstream outFile("last_write_time.dat", std::ios::binary);
    outFile.write(reinterpret_cast<const char*>(&timestampA), sizeof(timestampA));
    outFile.write(reinterpret_cast<const char*>(&timestampB), sizeof(timestampB));
    outFile.close();
}


void getFileDates(const std::string filePath) {
    std::cout << "File (" << filePath << ")\n";
    std::cout << "Readable: " << getLastModifiedReadable(filePath) << "\n";
    std::cout << "Numeric Local: " << getLastModifiedNumericLocal(filePath) << "\n";
    std::cout << "Numeric Raw: " << getLastModifiedNumericRaw(filePath) << "\n\n";
}


void compareFileDates(std::time_t& timestampA, std::time_t& timestampB) {
    std::cout << "File Date Comparison: ";
    if(timestampB > timestampA) {
        std::cout << timestampB << " > " << timestampA << "\n";
        std::cout << "File B is newer than File A.\n" << std::endl;
    } else if (timestampA > timestampB) {
        std::cout << timestampA << " > " << timestampB << "\n";
        std::cout << "File A is newer than File B.\n" << std::endl;
    } else {
        std::cout << timestampA << "=" << timestampB << "\n";
        std::cout << "File A and File B have the same last modified time.\n" << std::endl;
    }   
} 


void readFileTimes(std::time_t& timestampA, std::time_t& timestampB) {
    std::ifstream inpFile("last_write_time.dat", std::ios::binary);
    inpFile.read(reinterpret_cast<char*>(&timestampA), sizeof timestampA);
    inpFile.read(reinterpret_cast<char*>(&timestampB), sizeof timestampB);
    std::cout << "Timestamps Read from binary file:\n";
    std::cout << "File A Raw Timestamp: " << timestampA << "\n";
    std::cout << "File B Raw Timestamp: " << timestampB << "\n";
    inpFile.close();
}   


int main(int argc, char** argv) {
    (void)argc;  // Suppresses the warning without side effects

    // get executable directory parent path
    std::filesystem::path exe_dir_par = std::filesystem::path(argv[0]).parent_path();
    exe_dir_par = exe_dir_par.parent_path();

    // get file date/times
    std::filesystem::path fs_filePathA = exe_dir_par / "llm/matchQA/data/database_questions_answers.csv";
    std::filesystem::path fs_filePathB = exe_dir_par / "llm/matchQA/results/results.csv";
    //  Alternatively, use relative paths

    std::string filePathA = fs_filePathA.string();
    std::string filePathB = fs_filePathB.string();
    getFileDates(filePathA);
    getFileDates(filePathB);

    // Local numeric timestamp comparison
    std::time_t timestamp_csv_A_local = getLastModifiedNumericLocal(filePathA);
    std::time_t timestamp_csv_B_local = getLastModifiedNumericLocal(filePathB);
    std::cout << "Compare the csv file timestamps local format (A vs B):\n";
    compareFileDates(timestamp_csv_A_local, timestamp_csv_B_local);

    // Raw numeric timestamp comparison
    std::time_t timestamp_csv_A_raw = getLastModifiedNumericRaw(filePathA);
    std::time_t timestamp_csv_B_raw = getLastModifiedNumericRaw(filePathB); 
    std::cout << "Compare the csv file timestamps raw format (A vs B):\n";
    compareFileDates(timestamp_csv_A_raw, timestamp_csv_B_raw);

    // temporarily set the file timestamps to current csv values so that function will read 
    // consistent type from file determined by auto keyword
    std::time_t timestamp_bin_A_raw_prev = timestamp_csv_A_raw;
    std::time_t timestamp_bin_B_raw_prev = timestamp_csv_A_raw;

    // read the file times back from binary file with arguments passed by reference
    readFileTimes(timestamp_bin_A_raw_prev, timestamp_bin_B_raw_prev);

    // Compare the file dates read from binary file with the date on the csv file
    std::cout << "\nCompare the csv file raw timestamps with the binary file timestamps:\n";
    std::cout << "File A:\n";
    compareFileDates(timestamp_bin_A_raw_prev, timestamp_csv_A_raw);
    std::cout << "File B:\n";
    compareFileDates(timestamp_bin_B_raw_prev, timestamp_csv_B_raw);

    //write the new file time to binary file
    writeFileTimes(timestamp_csv_A_raw, timestamp_csv_B_raw);

    system("pause");

    return 0;
}