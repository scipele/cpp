#include <iostream>

int main() {
    std::string str = "Hello, World!";
    
    // Pointer to the beginning of the string
    char *ptr_begin = &str[0];

    // Pointer to the 7th character (index 6) in the string
    char *ptr_middle = &str[6];

    // Pointer to the end of the string
    char *ptr_end = &str[str.length() - 1];

    std::cout << "First part of the string: " << std::string(ptr_begin, ptr_middle - ptr_begin) << std::endl;
    std::cout << "Second part of the string: " << std::string(ptr_middle, ptr_end - ptr_middle + 1) << std::endl;

    return 0;
}
