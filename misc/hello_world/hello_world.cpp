#include <iostream>


void print_blanks() {
    for (int i = 0; i < 5; ++i)
    {
        std::cout << "\n";
    }
}


int main()
{
    print_blanks();
    std::cout << "Hello world!\n";
    print_blanks();
  
    std::cout << std::endl;
    
    return 0;
}