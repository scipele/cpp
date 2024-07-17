#include <iostream>
#include <string>
#include <cctype>

std::string lcase(const std::string& str);
bool cmp_strg_w_case(const std::string& str1, const std::string& str2);
bool cmp_strg_wo_case(const std::string& str1, const std::string& str2);

int main() {
    std::string str1 = "Charlie";
    std::string str2 = "Charlie";
    std::string str3 = "charlie";
    std::string str4 = "Willy";
    
    std::cout << "\n";
    std::cout << "Comparison of Strings Considering the Case:\n";
    std::cout << "\tComparison of '" << str1 << "', and '" << str2 << "' = " << std::boolalpha << cmp_strg_w_case(str1, str2) << "\n";
    std::cout << "\tComparison of '" << str1 << "', and '" << str3 << "' = " << std::boolalpha << cmp_strg_w_case(str1, str3) << "\n";
    std::cout << "\tComparison of '" << str1 << "', and '" << str4 << "' = " << std::boolalpha << cmp_strg_w_case(str1, str4) << "\n";

    std::cout << "\n";
    std::cout << "Comparison of Strings Without Considering the Case:\n";
    
    std::cout << "\tComparison of '" << str1 << "', and '" << str2 << "' = " << std::boolalpha << cmp_strg_wo_case(str1, str2) << "\n";
    std::cout << "\tComparison of '" << str1 << "', and '" << str3 << "' = " << std::boolalpha << cmp_strg_wo_case(str1, str3) << "\n";
    std::cout << "\tComparison of '" << str1 << "', and '" << str4 << "' = " << std::boolalpha << cmp_strg_wo_case(str1, str4) << "\n";

    return 0;
}

bool cmp_strg_w_case(const std::string& str1, const std::string& str2) {
    return (str1 == str2);
}

bool cmp_strg_wo_case(const std::string& str1, const std::string& str2) {
    if (str1.size() != str2.size()) {
        return false; // Strings of different lengths cannot be equal
    }
    
    auto it1 = str1.begin();    //initialize auto iterators for each string
    auto it2 = str2.begin();
    
    while (it1 != str1.end() && it2 != str2.end()) {
        if (std::tolower(*it1) != std::tolower(*it2)) {
            return false;
        }
        ++it1;
        ++it2;
    }
    
    return true; // All characters matched
}
