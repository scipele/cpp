#include <iostream>
#include <fstream>  
#include <cstring>  

int main(int argc, char* argv[])
{
     // Check if exactly one argument is provided
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <string_to_tokenize>" << std::endl;
        return 1; // Return error code
    }
 
    // Get the string to tokenize from command-line argument
    char* str = argv[1];
    char *ptr;  //declare a pointer
    ptr = strtok(str, ",");     //sets the pointer equal to initial portion of the string
    
   // Open a CSV file for writing
    std::ofstream outputFile("c:/t/output.csv");

    if (!outputFile.is_open()) {
        std::cerr << "Error opening output file!" << std::endl;
        return 1; // Return error code
    }

    while (ptr != NULL)  // Loop thru the pointer parts until NULL value is reached i.e. Not Equal to Null
    {
        outputFile << ptr << std::endl; // Write each token to a new line in the CSV file
        ptr = strtok(NULL, ",");    // sets the pointer to the next token or 'piece of the string'
    }
    // Close the CSV file
    outputFile.close();

return 0;
}
