#include <iostream>
#include <string>

// This example shows how strings are stored in memory addresses for each letter or space
// Also the example shows how pointers can be defined to different parts of a string

int main() {
    std::string str = "Show Memory Address of Each Character";

    // Pointer to the beginning of the string
    char *ptr_begin = &str[0];

    // Pointer to the 5th character (index 4) in the string
    char *ptr_space = &str[4];

    // Pointer to the end of the string
    char *ptr_end = &str[str.length() - 1];

    for (int i=0; i<50; i++) std::cout << "\n"; // print several line feeds to clear out screen
    std::cout << "\n\n";

    std::cout << "Example to Show how a string is stored in c++:";
    std::cout << "\n\n";
    std::cout << "\t with string input -> " << str << "\n\n";


    std::cout << "Pointer to the beginning (ptr_begin): " << static_cast<void*>(ptr_begin) << " (" << *ptr_begin << ")\n";
    std::cout << "Pointer to the middle (ptr_space)   : " << static_cast<void*>(ptr_space) << " (" << *ptr_space << ")\n";
    std::cout << "Pointer to the end (ptr_end)        : " << static_cast<void*>(ptr_end) << " (" << *ptr_end << ")\n\n";

    std::cout << "First part of the string:  " << std::string(ptr_begin, ptr_space - ptr_begin ) << "\n";
    std::cout << "Second part of the string: " << std::string(ptr_space, ptr_end - ptr_space ) << "\n\n";

    // Next loop thru each character and print the memory address of each character
    std::cout << "Indx  Ascii   Char    Hexidecimal Address of Memory Address where data is stored";
    for (int i = 0; i < str.length(); ++i) {
        std::cout << "[" << i << "]\t" << static_cast<int>(str[i]) << "\t'" << str[i] << "'\t" << static_cast<void*>(&str[i]) << std::endl;
    }

    std::cout << std::endl;

    return 0;
}
