// ************ MAIN PROGRAM ***************************************************
//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | scripple_scramble.cpp                                       |
//| EntryPoint   | main                                                        |
//| Purpose      | Scramble or unscramble the contents of a file               |
//| Inputs       | Path to the file to be scrambled                            |
//| Outputs      | Success or error message                                    |
//| Dependencies | Standard C++ libraries                                      |
//| By Name,Date | T.Sciple, 03/21/2026                                        |


#include <iostream>
#include <string>
#include <fstream>
#include <vector>


// Function prototypes
int scripple_scramble(const std::string& filePath);


int main()
{
    std::cout << "***************************************************\n"
              << "************* Scripple's Scramble *****************\n"
              << "***************************************************\n\n";
    
    std::cout << "This is meant to be a fun program to scramble the contents of a file.\n"
              << "It is not meant to be used for any malicious purposes.\n"
              << "Make sure that you only run this program on a very close friend.\n"
              << "It works by XORing each byte with the fixed value 0xAF.\n"
              << "That is a simple reversible byte toggle, not encryption or an attempt to hide data.\n"
              << "Running the program a second time on the same file restores the original contents.\n\n"
              << "Enter the path to the file you want to scramble / unscramble: ";
    
    std::string filePath;
    std::getline(std::cin, filePath);
    

    int result = scripple_scramble(filePath);
    if (result != 0) {
        std::cerr << "An error occurred while scrambling the file.\n";
    } else {
        std::cout << "File scrambled or unscrambled successfully!\n";
    }

    return 0;
}


int scripple_scramble(const std::string& filePath) { 
    size_t bufferSize = 4096;
    
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filePath << "\n";
        return -1;
    }

    // Make sure that the file is a regular file and not a directory
    file.seekg(0, std::ios::end);
    if (file.tellg() == 0) {
        file.close();
        std::cerr << "The file is empty: " << filePath << "\n";
        return -1;
    }

    try {
        std::vector<char> buffer(bufferSize);
        std::streampos pos = 0;
        // Read and scramble the file in chunks
        while (true) {
            file.clear();                   // Reset stream state flags before seeking
            file.seekg(pos);
            file.read(buffer.data(), buffer.size());
            std::streamsize bytesRead = file.gcount();
            if (bytesRead == 0) break;

            for (std::streamsize i = 0; i < bytesRead; ++i) {
                buffer[i] ^= 0xAF; // Fixed-mask XOR: applying the same operation again restores the original byte.
            }
            // Write the scrambled chunk back to the file
            file.clear();                   // Reset again before switching to write
            file.seekp(pos);
            file.write(buffer.data(), bytesRead);
            file.flush();
            pos += bytesRead;
        }
        file.close();
    } catch (...) {
        if (file.is_open()) file.close();
        std::cerr << "File was already opened by another process. Please close it and try again.\n";
        return -1;
    }
    
    // return 0 to indicate success
    return 0;
}