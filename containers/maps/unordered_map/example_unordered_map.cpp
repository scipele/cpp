//| Item	     | Documentation Notes                                         |
//|--------------|-------------------------------------------------------------|
//| Filename     | example_unordered_map.cpp                                   |
//| EntryPoint   | main                                                        |
//| Purpose      | example showing how to use an unordered map                 |
//| Inputs       | n/a                                                         |
//| Outputs      | print examples to the screen                                |
//| Dependencies | see standard includes                                       |
//| By Name,Date | T.Sciple, 01/17/2025                                        |

#include <iostream>
#include <unordered_map>
#include <string>

int main() {
    std::unordered_map<std::string, int> ages;

    // Inserting elements
    ages.reserve(6);    // reserves space for 4 elements
    ages = {
            {"Hailey", 22},
            {"Tony", 51},
            {"Bob", 55},
            {"Joshua", 24},
            {"Mary", 48}
    };

    // Note that the item is overwrtten if another element inserted with
    // the same key
    ages["Mary"] = 49; 

    std::cout   << "This is an example showing how to use an unordered map:" << std::endl;

    // 1. Accessing elements
    std::cout   << "1. Access Elements by the key:\n"
                << "\tHailey's age: " << ages["Hailey"] << "\n";
    std::cout << std::endl;

    // 2. Checking if an element exists
    std::cout << "2. Check if key exists:\n";
    if (ages.count("Mary") > 0) {
        std::cout << "\tMary's age: " << ages["Mary"] << "\n";
    } else {
        std::cout << "\tMary not found." << "\n";
    }
    std::cout << std::endl;

    // 3. Iterating over elements
    std::cout   << "3. Print all key value pairs:\n";
    for (const auto& pair : ages) {
        std::cout << "\t" << pair.first << ": " << pair.second << "\n";
    }
    std::cout << std::endl;

    // 4. Determine the number of elements inserted in the map
    std::cout   << "4. the number of elements inserted into the map:\n"
                << "\tunordered_map no of elements: " << ages.size() << "\n";
    std::cout << std::endl;

    // 5. Erase one of the elements by key
    ages.erase("Bob");
    std::cout   << "5. Reprint to see if element 'Bob' was erased:\n";
    for (const auto& pair : ages) {
        std::cout << "\t" << pair.first << ": " << pair.second << "\n";
    }
    std::cout   << "\n\tSize is now: " << ages.size();
    std::cout << "\n" << std::endl;

   // 6. show the count of a specific key
    std::cout << "6. Count member functon:\n";
    std::cout << "\tCount of 'Mary' Key inserted: " <<  ages.count("Mary") << "\n";
    std::cout << "\tNote that the count is only 1 because duplicate keys are not allowed\n";
    std::cout << "\tCount of 'Bob' Key inserted: " <<  ages.count("Bob") << " because 'Bob' was erased\n\n";

    // 7. insert another element
    ages["Jimmy"] = 45;
    std::cout << "7. Size following insertion:\n";
    if (ages.count("Jimmy") > 0) {
        std::cout << "\tJimmy was added, age: " << ages["Jimmy"] << "\n";
    }
    std::cout << "\tSize is now " << ages.size() << "\n";
 
    system("pause");
    return 0;
}