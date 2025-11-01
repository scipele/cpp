#include <iostream>
#include <string>
#include <vector>

// Function Prototypes
int for_loop_example();
int for_each_char_in_str_example();
int for_each_string_in_vector_example();
int for_each_string_in_vector_example_with_iterator();
int whileLoopWithTwoIteratorsInParalell();
int do_while_loop_example();
int while_loop_example();
void pauseConsole();

int main() {
    for_loop_example();
    for_each_char_in_str_example();
    for_each_string_in_vector_example();
    for_each_string_in_vector_example_with_iterator();
    whileLoopWithTwoIteratorsInParalell();
    do_while_loop_example();
    while_loop_example();
    pauseConsole();
    return 0;
}

int for_loop_example() {
    // This is an example of a for loop
    std::cout << "\n1. Example 'for' loop print index 0-5:\n\t";

    for(int i=0; i<=5; i++) {
        std::cout << i << " ";
    }
    std::cout << "\n";
    return 0;
}

int for_each_char_in_str_example() {
    std::string str = "Print message with space btw characters";
    // Iterate through each character in the string
    std::cout << "\n2. Example 'for' loop, iterates thru each character('c') contained in string('str'):\n\t";
    for (char c : str) {
        std::cout << c << " ";
    }
    std::cout << "\n\n";
    return 0;
}

int for_each_string_in_vector_example() {
    std::vector<std::string> planets = { "Mercury", "Venus", "Earth", "Mars", "Saturn", "Neptune", "Pluto" };
    // Iterate through a vector of std::strings
    std::cout << "3. Example of a range based for loop with auto iterators not shown explicitely\n\t";
    for (std::string planet : planets) {
        std::cout << planet << ", ";
    }
    std::cout << std::endl;
    return 0;
}

int for_each_string_in_vector_example_with_iterator() {
    std::vector<std::string> RainbowColors = { "Red", "Orange", "Yellow", "Green", "Blue", "Purple" };

    // Iterate through a vector of std::strings
    std::cout << "\n4. Example For_each_string_in_vector_example_with_iterator Rainbow Colors\n\t";
    std::vector<std::string>::iterator itt = RainbowColors.begin();
    
    for (itt; itt != RainbowColors.end(); ++itt) {
        std::cout << *itt;  // Note that the dereferencing opertor nust be used to get the value stored at the memory address of the iterator
                            //  which The iterator is actually a pointer to the memory address where each element is stored
        if(itt != (RainbowColors.end()-1)) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
    return 0;
}

int whileLoopWithTwoIteratorsInParalell() {
    std::cout << "\n5. Example While Loop With Two Iterators In Paralell\n\t";
    // Given two separate Vectors with the same number of elements
    std::vector<int> RowNos = { 103, 105, 110, 345, 500 };
    std::vector<std::string> Desc = { "Pipe", "Pipe", "Steel", "Equip", "Indirect" };
    std::vector<int>::iterator it1 = RowNos.begin();
    std::vector<std::string>::iterator it2 = Desc.begin();
    while ( it1 != RowNos.end() && it2 != Desc.end() ) {
            std::cout << *it1 << ", " << *it2;
            if (it1 != RowNos.end()-1 && it2 != Desc.end()-1) {
                std::cout << " | ";
            }
            it1++;
            it2++;
    }
    std::cout << "\n\n";
    return 0;
}

int do_while_loop_example() {
    int num = 1;
    std::cout << "6. Example 'do-while' loop print 11 to 20:\n\t";
    num = 11;
    do {
        std::cout << num << " ";
        num++;
    } while (num <= 20);
    return 0;
}

int while_loop_example() {
    int num = 20;
   std::cout << "\n\n7. Example 'while' loop to print numbers 20-30:\n\t";
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
