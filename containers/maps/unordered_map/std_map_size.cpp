#include <iostream>
#include <unordered_map>

int main() {
    
    // Creating an unordered_map with integer
    // keys and string values
    std::unordered_map<int, std::string> um =
    {{1, "Geeks"}, {2, "For"}, {3, "C++"}};

    for (auto i : um) 
        std::cout << i.first << ": " << i.second
        << std::endl;
    return 0;
}