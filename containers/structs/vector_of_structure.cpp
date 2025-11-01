#include <iostream>
#include <vector>

// Define a structure
struct MyStruct {
    int id;
    double value;
    // You can add more members as needed
};

int main() {
    // Define a vector of MyStruct
    std::vector<MyStruct> myStructVector;

    // Add elements to the vector
    myStructVector.push_back({1, 3.14});
    myStructVector.push_back({2, 2.718});
    myStructVector.push_back({3, 1.618});

    // Access elements in the vector
    for (const auto& elem : myStructVector) {
        std::cout << "ID: " << elem.id << ", Value: " << elem.value << std::endl;
    }

    return 0;
}