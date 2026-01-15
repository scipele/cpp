//| Item	     | Documentation Notes                                         |
//|--------------|-------------------------------------------------------------|
//| Filename     | example_unordered_map.cpp                                   |
//| EntryPoint   | main                                                        |
//| Purpose      | example showing how to use an unordered map                 |
//| Inputs       | n/a                                                         |
//| Outputs      | print examples to the screen                                |
//| Dependencies | see standard includes                                       |
//| By Name,Date | T.Sciple, 01/14/2025                                        |

#include <iostream>
#include <unordered_map>
#include <string>
#include <iomanip>

int main() {
    std::unordered_map<std::string, int> ages;

    // Inserting elements
    ages.reserve(14);    // reserves space for elements
    ages = {
            {"Hailey", 22},
            {"Tony", 51},
            {"Chad", 55},
            {"Bill", 51},
            {"William", 55},
            {"Nathan", 51},
            {"Bob", 55},
            {"Josh", 24},
            {"Mary", 48}
    };

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

   // 5. show the count of a specific key
    std::cout << "5. Count member functon:\n";
    std::cout << "\tCount of 'Mary' Key inserted: " <<  ages.count("Mary") << "\n";
    std::cout << "\tNote that the count is only 1 because duplicate keys are not allowed\n";
    std::cout << "\tCount of 'Bob' Key inserted: " <<  ages.count("Bob") << " because 'Bob' was erased\n\n";

    // 6. insert another element
    ages["Charlie"] = 45;
    std::cout << "6. Size following insertion:\n";
    if (ages.count("Charlie") > 0) {
        std::cout << "\tCharlie was added, age: " << ages["Charlie"] << "\n";
    }
    std::cout << "\tSize is now " << ages.size() << "\n" << std::endl;

    // 7. Bucket information
    int bucket_count = ages.bucket_count();
    std::cout << "7. Bucket information:\n";
    std::cout << "\tBucket count: " << bucket_count << "\n";
    std::cout << "\tLoad factor: " << ages.load_factor() << "\n";
    std::cout << std::endl;

    // 8. Show hash values for keys
    std::cout << "8. Hash values for keys:\n";
    std::hash<std::string> hasher;
    for (const auto& pair : ages) {
        size_t hash = hasher(pair.first); 
        size_t calc_bucket = hash % bucket_count;
        std::cout << "\tHash of '"
        << std::setw(10) << std::left
        << pair.first + "'" << "= "
        << std::setw(20) << std::right << hash
        << " % "
        << bucket_count << " = " << calc_bucket << "\n";
    }
    std::cout << std::endl;

    // 9. Memory addresses and bucket indices
    std::cout << "9. Memory addresses and bucket indices of elements:\n";
    for (const auto& pair : ages) {
        std::cout   << "\tMem Addr=" << &pair << ", Key='" 
                    << std::setw(10) << std::left << pair.first + "'"
                    << "Bucket index " << ages.bucket(pair.first) << "\n";
    }
    std::cout << std::endl;

    // 10. Show bucket contents to demonstrate collision handling
    std::cout << "10. Bucket contents (showing how collisions are handled):\n";
    for (size_t i = 0; i < ages.bucket_count(); ++i) {
        std::cout << "\tBucket " << i << ": ";
        bool first = true;
        for (auto it = ages.begin(i); it != ages.end(i); ++it) {
            if (!first) std::cout << ", ";
            std::cout << "(addr: "
                    << std::setw(3) << std::left << &(*it)
                    << ")" << "'"
                    << std::setw(10) << std::left
                    << it->first + "', "
                    << "paired value = " << it->second;
            first = false;
        }
        if (first) std::cout << "(empty)";
        std::cout << "\n";
    }
    std::cout << std::endl;

    // 11. Erase one of the elements by key
    ages.erase("Bob");
    std::cout   << "11. Reprint to see if element 'Bob' was erased:\n";
    for (const auto& pair : ages) {
        std::cout << "\t" << pair.first << ": " << pair.second << "\n";
    }
    std::cout   << "\n\tSize is now: " << ages.size();
    std::cout << "\n" << std::endl;


    system("pause");
    return 0;
}