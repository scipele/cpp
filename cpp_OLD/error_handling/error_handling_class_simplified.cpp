#include <iostream>
#include <string>
#include <unordered_map>

// Define Error Codes
enum class ErrCode {
    SUCCESS = 0,
    ATTEMPTED_DIVISION_BY_ZERO,
};

// ErrorHandler class to handle and print error messages
class ErrorHandler {
public:
    ErrorHandler(ErrCode code, const std::string& functionName)
        : code(code), functionName(functionName) {
        printErrorMessage();
    }

private:
    ErrCode code;
    std::string functionName;

    // Function to get error message from error code
    std::string getErrorMessage() const {
        static const std::unordered_map<ErrCode, std::string> errorMessages = {
            {ErrCode::SUCCESS, "\nSuccess"},
            {ErrCode::ATTEMPTED_DIVISION_BY_ZERO, "\nError - Attempted division by zero"}
        };

        auto it = errorMessages.find(code);
        if (it != errorMessages.end()) {
            return it->second;
        } else {
            return "Unrecognized error code";
        }
    }

    void printErrorMessage() const {
        std::cout << getErrorMessage() << " in Function: " << functionName << std::endl;
    }
};






// Example function to simulate file reading
ErrCode proportionNumbers(double a, double b, double c ) {
    
    if (c == 0) {
        return ErrCode::ATTEMPTED_DIVISION_BY_ZERO;
    }

    double answer;
    answer = a * b / c;

    std::cout << "Answer = " << answer;
    
    // Simulate successful file read
    return ErrCode::SUCCESS;
}



int main() {

    // Test trying to divide by zero
    ErrCode ec;
    
    ec = proportionNumbers(100.0, 5.0, 20.0);
    ErrorHandler(ec, "Call No 1, proportionNumbers");

    ec = proportionNumbers(100.0, 5.0, 0);
    ErrorHandler(ec, "Call No 2, proportionNumbers");

    return 0;
}
