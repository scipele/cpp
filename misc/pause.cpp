#include <iostream>


// Function to pause the console window until a key is pressed
void pauseConsole();


int main() {
    // Call the function to pause the console window
    std::cout << "Example Function to pause in Windows or Linux" << std::endl;
    pauseConsole();

    return 0;
}


void pauseConsole() {
    // If Windows, use system("pause"); otherwise, use cin.get() to wait for user input
    #ifdef _WIN32
        system("pause");    
    #else
        std::cout << "Press Enter to continue...";
        std::cin.get();
    #endif
}