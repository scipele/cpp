#include <iostream>
//#include <string>

// Function to pause the console window until a key is pressed
void pauseConsole();


int main() {

    // Call the function to pause the console window
    pauseConsole();
    // Continue with other code
    std::cout << "This code executes after pausing the console." << std::endl;

    // Call the function to pause the console window
    pauseConsole();

    return 0;
}


// Function to pause the console window until a key is pressed
void pauseConsole() {
    
    // Pause the console window before exiting
    system("pause");

}
