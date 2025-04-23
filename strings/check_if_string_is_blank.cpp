#include <iostream>
#include <string>
#include <cctype>


int main() {
    std::string str1 = " ";
    
    std::cout << "\n";
    std::cout << "Comparison of Strings Considering the Case:\n";
    std::cout << "\tComparison of '" << str1 << "', and '" << "blank" << "' = " << std::boolalpha << (str1 == " ") << "\n";
    std::cout << "\tComparison of '" << str1 << "', and '" << "abc" << "' = " << std::boolalpha << (str1 == "abc") << "\n\n";

    str1 = "";
    std::cout << "\tComparison of '" << str1 << "', and '" << "empty" << "' = " << std::boolalpha << (str1 == "") << "\n";
    std::cout << "\tComparison of '" << str1 << "', and '" << "abc" << "' = " << std::boolalpha << (str1 == "abc") << "\n\n";

    str1 = " ";
    std::cout << "\tNot Comparison of '" << str1 << "', and '" << "empty" << "' = " << std::boolalpha << (str1 != " ") << "\n";


    return 0;
}

