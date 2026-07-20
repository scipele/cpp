/*
Compile Linux:
g++ -std=c++17 -o create_binary create_binary.cpp
Compile Windows:
g++ -std=c++17 -o create_binary.exe create_binary.cpp
*/

#include <iostream>
#include <fstream>
int main()
{
    // Create a binary file
    std::ofstream binaryFile("example.bin", std::ios::binary);
    if (!binaryFile)
    {
        std::cerr << "Error creating binary file." << std::endl;
        return 1;
    }
    // write all printable ascii characters to the binary file
    for (char c = 32; c < 127; ++c)
    {
        binaryFile.write(&c, sizeof(c));
    }
    std::cout << "Binary file created successfully." << std::endl;
    binaryFile.close();
    return 0;
}
