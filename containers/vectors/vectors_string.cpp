
#include <iostream>
#include <vector>
#include <string>
#include <iomanip> // Include the header for setw()
#include <algorithm>

// Declare Functions before main
void buildVector();
void buildCharArray();


int main() {
    buildVector();
    buildCharArray();
    return 0;
}


void buildVector() {
    std::vector<std::string> myVector;
    myVector.push_back("Mary");
    myVector.push_back(" ");
    myVector.push_back("L.");
    myVector.push_back(" ");
    myVector.push_back("Sciple");

    // Now add to the beginning of the vector
    myVector.insert(myVector.begin(), "Mrs. ");

    // Output the strings in the vector
    std::cout << "\n1. \033[4mVector Example:\033[0m\n";
    std::cout << "\t1.1 Vector Pieces:\n";    
    
    // Loop through the individual pieces of the string vector and print pieces sizes:
    int totSize = 0;
    int i = 0;
    // Note that the size here really only represents the size of the data
    for (const std::string& str : myVector) {
        std::cout << "\t\t|" << str << "|" << std::setw((20-str.size())) << std::right << "Piece [" << i << "]\t Size:\t" << str.size() << "\n";
        i++;
        totSize += str.size();
    }
    std::cout << "\t\tTotal Size of Pieces :" << std::setw(14) << totSize << "\n";
    std::cout << "\t1.2 Now printed without pipe between pieces:\n\t\t";
    for (const std::string& str : myVector) {
        std::cout << str;
    }

    std::cout << "\n\t1.3 Vector Size inluding overhead functions: " << sizeof(myVector) << " bytes\n";

    //Next delete pieces of the vector by iterator location:
    std::cout << "\t1.4 Next Delete Pieces of the Vector:\n";
    std::vector<std::string>::iterator it = myVector.begin() + 3;
    std::cout << "\t\tDelete this item: |" << *it << "|\n";
    myVector.erase(it);

    it = myVector.begin() + 2;
    std::cout << "\t\tDelete this item: |" << *it << "|\n";
    myVector.erase(it);

    it = myVector.begin() + 0;
    std::cout << "\t\tDelete this item: |" << *it << "|\n";
    myVector.erase(it);
    
    totSize = 0;
    std::cout << "\t1.5 Now reprint vector with some of the pieces removed:\n\t\t";
    for (const std::string& str : myVector) {
        std::cout << str;
        totSize += str.size();
    }

    std::cout << "\n\t1.6 Vector Size overhead functions only: " << sizeof(myVector) << " bytes\n";
    std::cout << "\t    This represents only the size of the vector but not data stored";

    std::cout << "\n\t1.7 Vector Ovhd Size (" << sizeof(myVector) << ") + Size of data pieces (" <<  totSize << ")= " << sizeof(myVector) + totSize << " bytes\n";

}


void buildCharArray() {
    // Create a character array and initialize it with "Mrs. Mary L. Sciple"
    char mycharArray[] = "Mrs. Mary L. Sciple";

    // Output the character array
    std::cout << "\n2. \033[4mCharacter Array Example:\033[0m\n";
    std::cout << "\t2.1 Printed Char Array:\n\t\t";    
    std::cout << mycharArray;
    std::cout << "\n\t2.2 Char Array Size: " << sizeof(mycharArray) << " bytes which includes a terminating null at the end \\'000'\n";
}