#include <iostream>
#include <string>
#include <unordered_map>

// Error handling class
class ErrorHandler {
public:
    // Define Error Codes
    enum class ErrorCode {
        SUCCESS = 0,
        FILE_NOT_FOUND,
        INVALID_PARAMETER,
        OUT_OF_MEMORY,
        UNKNOWN_ERROR,
    };

    // Constructor
    ErrorHandler(ErrorCode code, const std::string& functionName)
        : code(code), functionName(functionName) {
        printErrorMessage();
    }

    // Assignment operator
    ErrorHandler& operator=(ErrorCode newCode) {
        code = newCode;
        printErrorMessage();
        return *this;
    }

    // Get the error code
    ErrorCode getCode() const {
        return code;
    }

    // Example function to simulate file reading
    static ErrorHandler readFile(const std::string& filename) {
        if (filename.empty()) {
            return ErrorHandler(ErrorCode::INVALID_PARAMETER, "readFile");
        }

        // Simulate file reading
        bool fileExists = false; // Change as needed
        if (!fileExists) {
            return ErrorHandler(ErrorCode::FILE_NOT_FOUND, "readFile");
        }

        // Simulate successful file read
        return ErrorHandler(ErrorCode::SUCCESS, "readFile");
    }

    // Example function to simulate memory allocation
    static ErrorHandler allocateMemory(int size) {
        if (size <= 0) {
            return ErrorHandler(ErrorCode::INVALID_PARAMETER, "allocateMemory");
        }

        // Simulate memory allocation failure
        bool memoryAllocated = false; // Change as needed
        if (!memoryAllocated) {
            return ErrorHandler(ErrorCode::OUT_OF_MEMORY, "allocateMemory");
        }

        // Simulate successful memory allocation
        return ErrorHandler(ErrorCode::SUCCESS, "allocateMemory");
    }

private:
    ErrorCode code;
    std::string functionName;

    // Function to get error message from error code
    std::string getErrorMessage() const {
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

    void printErrorMessage() const {
        std::cout << "Error in function '" << functionName << "': "
                  << getErrorMessage() << std::endl;
    }
};

int main() {
    // Test readFile function
    ErrorHandler result = ErrorHandler::readFile("example.txt");

    result = ErrorHandler::readFile("");

    // Test allocateMemory function
    result = ErrorHandler::allocateMemory(1024);

    result = ErrorHandler::allocateMemory(-1);

    return 0;
}
