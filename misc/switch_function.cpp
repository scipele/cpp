#include<iostream>
#include<string>
#include <sstream>

// Enums are a way of assigning an integer value to a phrase
enum EstFactors
{
  FIELD_PIPE_WELD_CS = 1, 
  FIELD_PIPE_WELD_ALLOY = 2, 
  FIELD_PIPE_ERECT = 3
};

// Function Prototypes:
double GetFactorForChoice(int& categ_choice);


int main() {

    std::cout << "Enter Choice Below to return the factor:\n";
    std::cout << "\tEnter 1 for FIELD_PIPE_WELD_CS\n";
    std::cout << "\tEnter 2 for FIELD_PIPE_WELD_ALLOY\n";
    std::cout << "\tEnter 3 for FIELD_PIPE_ERECT\n";
    std::cout << "\tEnter 0 - Exit\n";
    int categ_choice = 1;


    double factor;
    std::string input;

    while (categ_choice != 0) {
        
        std::getline(std::cin, input);
        std::stringstream ss(input);

        factor = GetFactorForChoice(categ_choice);
        std::cout << "\tFactor: " << factor << "\n";
    }

    return 0;
}

double GetFactorForChoice(int& categ_choice) {
    switch (categ_choice)
    {
    case FIELD_PIPE_WELD_CS:    // Same as writing Case 1:
        return 1.0;
    case FIELD_PIPE_WELD_ALLOY: // Same as writing Case 2:
        return 2.75;
    case FIELD_PIPE_ERECT:      // Same as writing Case 3:
        return 1.25;
    default:
        return 0;
        std::cout << "\n\nInvalid Selection\n";
        break;
    }
}