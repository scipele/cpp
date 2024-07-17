#include <iostream>
#include <string>

// This example shows how strings are stored in memory addresses for each letter or space
// Also the example shows how pointers can be defined to different parts of a string

int main() {
    std::string str = "Hello, World!";

    // Pointer to the beginning of the string
    char *ptr_begin = &str[0];

    // Pointer to the 7th character (index 6) in the string
    char *ptr_middle = &str[6];

    // Pointer to the end of the string
    char *ptr_end = &str[str.length() - 1];

    std::cout << "Pointer to the beginning (ptr_begin): " << static_cast<void*>(ptr_begin) << " (" << *ptr_begin << ")\n";
    std::cout << "Pointer to the middle (ptr_middle)  : " << static_cast<void*>(ptr_middle) << " (" << *ptr_middle << ")\n";
    std::cout << "Pointer to the end (ptr_end)        : " << static_cast<void*>(ptr_end) << " (" << *ptr_end << ")\n";

    std::cout << "First part of the string:  " << std::string(ptr_begin, ptr_middle - ptr_begin) << std::endl;
    std::cout << "Second part of the string: " << std::string(ptr_middle, ptr_end - ptr_middle + 1) << std::endl;

    // Next loop thru each character and print the memory address of each character
    for (int i = 0; i < str.length(); ++i) {
        std::cout << "[" << i << "]\t" << static_cast<int>(str[i]) << "\t'" << str[i] << "'\t" << static_cast<void*>(&str[i]) << std::endl;
    }

    return 0;
}
