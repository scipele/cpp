#include <iostream>
#include <vector>

void addToVector(std::vector<char>& myvector);

int main() {
    std::vector<char> myVector = {'M', 'a', 'r', 'y'};
    addToVector(myVector); // Pass the vector to the function for modification

    // Output the modified vector
    for (char c : myVector) {
        std::cout << c;
    }
    std::cout << std::endl;
    return 0;
}


void addToVector(std::vector<char>& myvector) {     // Note that its passed by reference
    myvector.push_back(' '); // Modify the vector by adding a single element    
    myvector.push_back('L'); 
    myvector.push_back('.');
    myvector.push_back(' '); 
    myvector.insert(myvector.end(), { 'S', 'c', 'i', 'p','l','e' }); // Modify the vector by adding multiple elements to end
    myvector.insert(myvector.begin(), { 'M', 'r', 's', '.',' ' }); // Modify the vector by adding multiple elements to beginning
}

