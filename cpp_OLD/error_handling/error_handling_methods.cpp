#include <iostream>
#include <string>
#include <unordered_map>

// Define Error Codes
enum class ErrorCode {
    SUCCESS = 0,
    FILE_NOT_FOUND,
    INVALID_PARAMETER,
    OUT_OF_MEMORY,
    UNKNOWN_ERROR,
};

// Function to get error message from error code
std::string getErrorMessage(ErrorCode code) {
    static const std::unordered_map<ErrorCode, std::string> errorMessages = {
        {ErrorCode::SUCCESS, "Success"},
        {ErrorCode::FILE_NOT_FOUND, "File not found"},
        {ErrorCode::INVALID_PARAMETER, "Invalid parameter"},
        {ErrorCode::OUT_OF_MEMORY, "Out of memory"},
        {ErrorCode::UNKNOWN_ERROR, "Unknown error"}
    };

    auto it = errorMessages.find(code);
    if (it != errorMessages.end()) {
        return it->second;
    } else {
        return "Unrecognized error code";
    }
}

// Error Wrapper Class
class ErrorWrapper {
public:
    // Constructor
    ErrorWrapper(ErrorCode code, const std::string& functionName)
        : code(code), functionName(functionName) {
        printErrorMessage();
    }

    // Assignment operator
    ErrorWrapper& operator=(ErrorCode newCode) {
        code = newCode;
        printErrorMessage();
        return *this;
    }

    // Get the error code
    ErrorCode getCode() const {
        return code;
    }

private:
    ErrorCode code;
    std::string functionName;

    void printErrorMessage() const {
        std::cout << "Error in function '" << functionName << "': "
                  << getErrorMessage(code) << std::endl;
    }
};

// Example function to simulate file reading
ErrorWrapper readFile(const std::string& filename) {
    if (filename.empty()) {
        return ErrorWrapper(ErrorCode::INVALID_PARAMETER, "readFile");
    }

    // Simulate file reading
    bool fileExists = false; // Change as needed
    if (!fileExists) {
        return ErrorWrapper(ErrorCode::FILE_NOT_FOUND, "readFile");
    }

    // Simulate successful file read
    return ErrorWrapper(ErrorCode::SUCCESS, "readFile");
}

// Example function to simulate memory allocation
ErrorWrapper allocateMemory(int size) {
    if (size <= 0) {
        return ErrorWrapper(ErrorCode::INVALID_PARAMETER, "allocateMemory");
    }

    // Simulate memory allocation failure
    bool memoryAllocated = false; // Change as needed
    if (!memoryAllocated) {
        return ErrorWrapper(ErrorCode::OUT_OF_MEMORY, "allocateMemory");
    }

    // Simulate successful memory allocation
    return ErrorWrapper(ErrorCode::SUCCESS, "allocateMemory");
}

int main() {
    // Test readFile function
    ErrorWrapper result = readFile("example.txt");

    result = readFile("");

    // Test allocateMemory function
    result = allocateMemory(1024);

    result = allocateMemory(-1);

    return 0;
}
