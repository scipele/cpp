#include <iostream>
#include <string>

int for_loop_example() {
    // This is an example of a for loop
    std::cout << "\nExample 'for' loop print index 0-5:\n\t";

    /* syntax
    for (initialization; condition; iteration) {
        statements to be executed
    }*/

    for(int i=0; i<=5; i++) {
        std::cout << i << " ";
    }
    return 0;
}

int for_each_char_in_str_example() {
    std::string str = "Print  message with space btw characters";

    // Iterate through each character in the string
    std::cout << "\n\n\nExample 'for' loop, iterates thru each character('c') contained in string('str'):\n\t";
    for (char c : str) {
        std::cout << c << " ";
    }
    std::cout << std::endl;

    return 0;
}

int do_while_loop_example() {
    int num = 1;

    std::cout << "\n\nExample 'do-while' loop print 11 to 20:\n\t";
    num = 11;
    do {
        std::cout << num << " ";
        num++;
    } while (num <= 20);

    return 0;
}


int while_loop_example() {
    int num = 20;

    std::cout << "\n\n\nExample 'while' loop to print numbers 20-30:\n\t";
    while (num <= 30) {
        std::cout << num << " ";
        num++;
    }

    std::cout << "\n\n";

    return 0;
}

// Function to pause the console window until a key is pressed
void pauseConsole() {
    std::cout << "Press any key to continue...";
    std::string input;
    std::getline(std::cin, input); // Wait for the user to press Enter
}


int main() {

    for_loop_example();
    for_each_char_in_str_example();
    do_while_loop_example();
    while_loop_example();
    pauseConsole();

    return 0;
}