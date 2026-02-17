// ************ MAIN PROGRAM ***************************************************
//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | kaprekars_const.cpp                                         |
//| EntryPoint   | main                                                        |
//| Purpose      | illustrate Kaprekar's constant                              |
//| Inputs       | Any four digit number except like 1111, 2222, etc.          |
//| Outputs      | iterations to reach Kaprekar's constant                     |
//| Dependencies | none                                                        |
//| By Name,Date | T.Sciple, 2/16/2026                                         |

#include<iostream>
#include<string>
#include <algorithm>
#include <iomanip>


//Function prototypes
std::string padWithZeros(const std::string& str);
int printKaprekarIterations(const std::string& str);


int main()
{
    std::cout << "Kaprekar's constant Definition:\n";
    std::cout   << "Kaprekar's constant is 6174, a unique four-digit number in base 10 that acts as a mathematical black hole. Named after Indian mathematician D. R. Kaprekar in 1949,\n"
                << "this constant is reached by repeatedly applying a specific, simple arithmetic routine to almost any four-digit number.\n\n"
                << "How it works:\n"
                << "To reach the constant 6174, follow these steps:\n"
                << "1. Choose a 4-digit number: Take any four-digit number, ensuring at least two digits are different (leading zeros are allowed, e.g., 123 is treated as 0123).\n"
                << "2. Rearrange: Arrange the digits in descending order (largest number) and ascending order (smallest number).\n"
                << "3. Subtract: Subtract the smaller number from the larger number.\n"
                << "Repeat: Take the resulting number and repeat the process. \n"
                << "Within a maximum of seven iterations, the result will always be 6174. \n\n";

    std::string s;
    std::cout << "Enter a 4-digit number: ";
    std::getline(std::cin, s);

    printKaprekarIterations(s); // Call the function to perform the iterations

    for (int i = 0; i < 10; i++) {
        // try some random numbers
        int randomNum = rand() % 10000; // Generate a random number between 0 and 9999
        std::string randomStr = std::to_string(randomNum);
        printKaprekarIterations(randomStr); // Call the function to perform the iterations  
    }

    std::cout << "Press Enter to continue...";
    std::cin.get(); // Wait for user input before closing the console
    
    return 0;
}



int printKaprekarIterations(const std::string& str) {
    std::string s = str;
    s = s.substr(0, 4);                                                     // Ensure we only take the first 4 characters

    int n = 0;
    int indx = 0;
    std::cout << "\nStarting number: " << s << "\n";
    std::cout << "+------+-------+-------+-------+-------+\n"
              << "| iter | start | large | small | delta |\n"
              << "+------+-------+-------+-------+-------+\n";

    while (n != 6174 && indx != 7)
    {
        s = padWithZeros(s);                                                // Pad with zeros if necessary
        std::string s_desc = s;                                             // Create a copy for descending order
        std::string num_desc = padWithZeros(s_desc);                        // Convert to string
        std::sort(num_desc.begin(), num_desc.end(), std::greater<char>());  // Sort in descending order

        std::string s_asc = s;                                              // Create a copy for ascending order
        // sort asccending order
        std::string num_asc = padWithZeros(s_asc);                          // Convert to string    
        std::sort(num_asc.begin(),num_asc.end(), std::less<char>());        // Sort in ascending order

        int delta = std::stoi(num_desc) - std::stoi(num_asc);               // Calculate the difference

        indx++;
        std::cout   << "|  " << indx << "  "
                    << " |" << std::setw(6) << s
                    << " |" << std::setw(6) << num_desc
                    << " |" << std::setw(6) << num_asc
                    << " |" << std::setw(6) << delta << " |\n";

        s = std::to_string(delta);                                          // Convert back to string for the next iteration
        n = std::stoi(s);                                                   // Update n for the loop condition
    }
    
    if (n != 6174) {
        std::cout << "\nKaprekar's constant was not reached within 7 iterations." << "\n";
    } else {
        std::cout << "\nKaprekar's constant reached in " << indx << " iterations: " << s << "\n";
    }

    return 0;  // exit normally
}


std::string padWithZeros(const std::string& str) {
    int n = std::stoi(str);                 // Convert to a number to remove leading zeros if present
    std::string padded = std::to_string(n);
    while (padded.length() < 4) {
        padded = padded + "0";              // Pad with zeros on the right
    }
    return padded;
}