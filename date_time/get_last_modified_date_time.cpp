#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <iostream>
#include <string>

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
long getLastModifiedNumeric(const std::string& filePath) {
    auto lastWriteTime = std::filesystem::last_write_time(filePath);
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        lastWriteTime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
    );
    auto tt = std::chrono::system_clock::to_time_t(sctp);
    return static_cast<long>(tt);
}


int main() {
    std::string filePath = "cpp/llm/matchQA/database_questions_answers.csv";
    
    std::cout << "Readable: " << getLastModifiedReadable(filePath) << std::endl;
    std::cout << "Numeric: " << getLastModifiedNumeric(filePath) << std::endl;
    
    return 0;
}