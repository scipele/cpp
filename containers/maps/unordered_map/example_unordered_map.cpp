#include <iostream>
#include <unordered_map>
#include <string>

int main() {
    std::unordered_map<std::string, int> ages;

    // Inserting elements
    ages["Alice"] = 25;
    ages["Bob"] = 30;
    ages["Charlie"] = 35;

    // Accessing elements
    std::cout << "Alice's age: " << ages["Alice"] << std::endl;

    // Checking if an element exists
    if (ages.count("David") > 0) {
        std::cout << "David's age: " << ages["David"] << std::endl;
    } else {
        std::cout << "David not found." << std::endl;
    }

    // Iterating over elements
    for (const auto& pair : ages) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }

    return 0;
}