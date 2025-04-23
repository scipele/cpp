#include <iostream>
#include <string>
#include <unordered_map>

// Define Error Codes
enum class ErrCode {
    SUCCESS = 0,
    ATTEMPTED_DIVISION_BY_ZERO,
    INVALID_ARGUMENT_PROVIDED_FOR_PARAM3,
    INVALID_ARGUMENT_PROVIDED_FOR_TYPE,
    GENERAL_ERROR,
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
            {ErrCode::SUCCESS, "\n\t*** Success ***"},
            {ErrCode::INVALID_ARGUMENT_PROVIDED_FOR_PARAM3, "\n\t*** Error *** Invalid argument provided for numeric param number 3"},
            {ErrCode::INVALID_ARGUMENT_PROVIDED_FOR_TYPE, "\n\t*** Error *** Invalid Argument Provided For Type"},
            {ErrCode::GENERAL_ERROR, "\n\t*** Error *** General Error"},
            {ErrCode::ATTEMPTED_DIVISION_BY_ZERO, "\n\t*** Error *** Attempted division by zero"}
        };

        auto it = errorMessages.find(code);
        if (it != errorMessages.end()) {
            return it->second;
        } else {
            return "\n\t*** Error *** Unrecognized error code";
        }
    }
    // Private Function that prints the error message and the 
    void printErrorMessage() const {
        std::cout << getErrorMessage() << " in Function: " << functionName << "\n" << std::endl;
    }
};

ErrCode proportionNumbers(double a, double b, double c);
ErrCode concreteVolume(std::string type, double param1_ft, double param2_ft, double param3_ft );
double near_zero(double num);

int main() {

    std::cout << std::string(30, '\n');    // clear screen
    std::cout << std::endl;

    // Test trying to divide by zero
    ErrCode ec; // Declare an instance of enum Class ErrCode

    std::cout << "1. Proportion Numbers:";    
    ec = proportionNumbers(100.0, 5.0, 20.0);
    ErrorHandler(ec, "Call No 1, proportionNumbers");

    std::cout << "2. Proportion Numbers:";    
    ec = proportionNumbers(100.0, 5.0, 0);
    ErrorHandler(ec, "Call No 2, proportionNumbers");

    std::cout << "3. Conrete Volume Calculation:";    
    ec = concreteVolume("cylinder", 10.0, 5.0, 0.0);
    ErrorHandler(ec, "Call No 1, concreteVolume");

    std::cout << "4. Conrete Volume Calculation:";    
    ec = concreteVolume("rectangle", 10.0, 5.0, 5.0);
    ErrorHandler(ec, "Call No 2, concreteVolume");

    std::cout << "5. Conrete Volume Calculation:";    
    ec = concreteVolume("octagon", 20.0, 10.0, 0.0);
    ErrorHandler(ec, "Call No 3, concreteVolume");

    std::cout << "6. Conrete Volume Calculation:";    
    ec = concreteVolume("octagon", 0.0, 10.0, 0.0);
    ErrorHandler(ec, "Call No 4, concreteVolume");

    return 0;
}

// Example function to simulate file reading
ErrCode proportionNumbers(double a, double b, double c ) {

    if (c == 0) {
        return ErrCode::ATTEMPTED_DIVISION_BY_ZERO;
    }

    double answer;
    answer = a * b / c;

    std::cout << "\n\tAnswer = " << answer;
    
    // Simulate successful file read
    return ErrCode::SUCCESS;
}

double near_zero(double num) {
    double abs_num;
    if (num < 0) {
        abs_num = -1 * num;
    } else {
        abs_num = num;
    }

    if (abs_num < 0.000001) {
        return true;
    } else {
        return false;
    }
}

ErrCode concreteVolume(std::string type, double param1_ft, double param2_ft, double param3_ft ) {

    int caseNum;
    double calc_vol_cf;

    if (type == "cylinder") caseNum = 1;
    if (type == "rectangle") caseNum = 2;
    if (type == "octagon") caseNum = 3;
    if ( caseNum != 1 && caseNum != 2 && caseNum != 3 ) {
        caseNum = -1;  // set to -1 to push back error with default in switch statement
    }

    switch (caseNum)
    {
        case 1:
        {
            double PI = 3.14159;
            calc_vol_cf = PI / 4 * ( param1_ft * param1_ft ) * param2_ft / 27;

            if (!near_zero(param3_ft)) {
                return ErrCode::INVALID_ARGUMENT_PROVIDED_FOR_PARAM3;
            }
            if (near_zero(calc_vol_cf)) {
                return ErrCode::GENERAL_ERROR;
            }            
            // "cylinder" param:  1=Diameter, 2=Length, 3=Not Used
            std::cout   << "\n\tConcrete Volume for Cylinder Shape = \n" 
                        << "\t\tVolumne = PI      / 4 x Diameter^2 x Length / 27\n" 
                        << "\t\t        = " << PI << " / 4 x " << param1_ft << "^2 x " << param2_ft << " / 27 = " << calc_vol_cf << "\n";
            return ErrCode::SUCCESS;
            break;
        }
        case 2:
        {
            calc_vol_cf = param1_ft * param2_ft * param3_ft / 27;
            if (near_zero(calc_vol_cf)) {
                return ErrCode::GENERAL_ERROR;
            }        
            std::cout   << "\n\tConcrete Volume for Rectangular Shape = \n"
                        << "\t\tVolumne = Length x Width x Depth / 27\n" 
                        << "\t\t        = " << param1_ft << " x " << param2_ft << " x " << param3_ft << " = " << calc_vol_cf << "\n";
            return ErrCode::SUCCESS;
            break;
        }
        case 3:
        {
            if (!near_zero(param3_ft)) {
                return ErrCode::INVALID_ARGUMENT_PROVIDED_FOR_PARAM3;
            }
            calc_vol_cf = 0.8284 * (param1_ft * param1_ft) * param2_ft / 27;
            if (near_zero(calc_vol_cf)) {
                return ErrCode::GENERAL_ERROR;
            }        
            std::cout   << "\n\tConcrete Volume for Octagonal Shape = \n"
                        << "\t\tVolumne = 0.8284 x Flats Dimension^2 x Depth / 27\n" 
                        << "\t\t        = 0.8284 x " << param1_ft << "^2 x " << param2_ft << " = " << calc_vol_cf << "\n";
            return ErrCode::SUCCESS;
            break;
        }
        default:
        {
            return ErrCode::INVALID_ARGUMENT_PROVIDED_FOR_TYPE;
            break;
        }
    }

    return ErrCode::SUCCESS;
}