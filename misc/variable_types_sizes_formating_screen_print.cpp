#include<iostream>
#include<string>
#include <iomanip>

std::string FormatWithCommas(long long int num);
std::string FormatWithCommas(int num);
std::string FormatWithCommas(size_t num);

//This program provides examples of working with various variable types

int main() {

    std::cout << "Various Variable Types:\n";
    unsigned char a = 255;  // Unsigned character can be used to store a small number that is a single byte (0-255)
    std::cout << std::setw(40) << std::left << "\t1. unigned characator 'a'" << std::setw(40) << "a = " + std::to_string(int{a}) << "size = " + std::to_string(sizeof(a)) + " bytes" << "\n";

    short int b = 32767;  // max size is based on a +/- 32,767 which is 2^16 (2 byte)
    std::cout << std::setw(40) << std::left << "\t2. short integer 'short int'" << std::setw(40) << "b = " + FormatWithCommas(b) << std::setw(15) << "size = " + std::to_string(sizeof(b)) + " bytes" << "\n";

    unsigned short int b2 = 65535;  // max size is based on a + 65,535 which is 2^16 - 1 for zero (2 byte)
    std::cout << std::setw(40) << std::left << "\t2.1 unigned short integer 'short int'" << std::setw(40) << "b2 = " + FormatWithCommas(b2) << std::setw(15) << "size = " + std::to_string(sizeof(b2)) + " bytes" << "\n";

    int c = 2147483647;  // max size is based on a +/- 2,147,483,647 which is (2^32/2 -1)  (4 byte) 
    std::cout << std::setw(40) << std::left << "\t3. integer 'int'" << std::setw(40) << "c = " + FormatWithCommas(c) << "size = " + std::to_string(sizeof(c)) + " bytes" << "\n";

    long long int d = 9223372036854775807;  // max size is based on a +/- 9,223,372,036,854,775,807 which is (2^64/2 -1)  (4 byte)
    std::cout << std::setw(40) << std::left << "\t4. integer 'long long int'" << std::setw(40) << "d = " + FormatWithCommas(d) << "size = " + std::to_string(sizeof(d)) + " bytes" << "\n";

    char e = 't';
    std::cout << std::setw(40) << std::left << "\t5. characator 'e'" << std::setw(40) << "e = " + std::string(1, e) << "size = " + std::to_string(sizeof(e)) + " bytes" << "\n";

    float f = 3.14159785;  
    std::cout << std::setw(40) << std::left << "\t6. float" << std::setw(40) << "f = " +  std::to_string(f) << "size = " + std::to_string(sizeof(f)) + " bytes" << "\n";

    double g = 3.14159785;  
    std::cout << std::setw(40) << std::left << "\t7. double" << std::setw(40) << "g = " +  std::to_string(g) << "size = " + std::to_string(sizeof(g)) + " bytes" << "\n";

    bool h = true;
    std::cout << std::setw(40) << std::left << "\t8. boolean (0-false, 1=true)" << std::setw(40) << "h = " +  std::to_string(h) << "size = " + std::to_string(sizeof(h)) + " bytes" << "\n";

    size_t j = 9223372036854775807;
    std::cout << std::setw(40) << std::left << "\t9. size_t" << std::setw(40) << "j = " +  FormatWithCommas(j) << "size = " + std::to_string(sizeof(j)) + " bytes" << "\n";
    
    char str[] = "Jack Smith";
    std::cout << std::setw(40) << std::left << "\t10. Char Array" << std::setw(40) << "str = " + std::string(str)<< "size = " + std::to_string(sizeof(str)) + " bytes" << "\n";
    
    // Next this will loop thru each character in string array and add space in between letters
    std::string str2;
    for(size_t i=0; i < sizeof(str)-1; i++) {
        
        str2 += str[i];
        if(i != sizeof(str)-2 ) {  // only add space between letters until on the last iteration
            str2 += " ";
        }
    }

    std::cout << std::setw(40) << std::left << "\t11. Loop Thru Char Array + space" << std::setw(40) << std::left << "str2 = " + str2 << "size = " + std::to_string(sizeof(str2)) + " bytes with spaces" << "\n";

    std::string str3 = "Hello World!";
    std::cout << std::setw(40) << std::left << "\t12. Standard String" << std::setw(40) << std::left << "str3 = " + str3 << "size = " + std::to_string(sizeof(str3)) + " bytes" << ", Length = " + std::to_string(str3.length()) + " characters, capacity = " << str3.capacity() <<"\n";

    //this is a string literal
    //these can be useful for multiple line strings that contain newline characters
    // \t = tab,  \n=newline
    std::string mlineStr = "Example Multiline\n" \
                                  "\t\t\t\t\t\tstring)";

    std::cout << std::setw(40) << std::left <<  "\t13. Multiline string:" << std::setw(40) << std::left << "mlineStr = " + mlineStr << "\t\t\t\t  size = " + std::to_string(sizeof(mlineStr)) + " bytes" << ", Length = " + std::to_string(mlineStr.length()) + " characters, capacity = " << mlineStr.capacity() <<"\n";

//boolean (means true or false)    
    int age = 19;
    bool over_21;
    if (age >= 21) {
        over_21 = true;
    } else {
        over_21 = false;
    }
    std::cout << std::setw(40) << std::left << "\t14. Boolean Printed as Alpha" << std::setw(40) << std::left << "Is person over 21 = " + std::string(over_21 ? "true" : "false") << "size = " + std::to_string(sizeof(over_21)) + " bytes\n";
    return 0;
}


std::string FormatWithCommas(int num) {
    std::string str = std::to_string(num);
    int bkw_cnt = str.length(); // initialize a backward counter to count down from string len to 0
    std::string formated_str;

    for (char c : str) {     // loops thru each character in the string
        formated_str += c;
        bkw_cnt--;  // deincrement the count by 1 after the first character which will always be there before checking for needed commas
        if (!bkw_cnt == 0) {
            if(bkw_cnt % 3 == 0) {  // checks if the mod of bkw_cnt / 3 == 0, or remainder is 0
                    formated_str += ',';
            }
        }
    }
    return formated_str;
}

std::string FormatWithCommas(long long int num) {
    std::string str = std::to_string(num);
    int bkw_cnt = str.length(); // initialize a backward counter to count down from string len to 0
    std::string formated_str;

    for (char c : str) {     // loops thru each character in the string
        formated_str += c;
        bkw_cnt--;  // deincrement the count by 1 after the first character which will always be there before checking for needed commas
        if (!bkw_cnt == 0) {
            if(bkw_cnt % 3 == 0) {  // checks if the mod of bkw_cnt / 3 == 0, or remainder is 0
                    formated_str += ',';
            }
        }
    }
    return formated_str;
}

std::string FormatWithCommas(size_t num) {
    std::string str = std::to_string(num);
    int bkw_cnt = str.length(); // initialize a backward counter to count down from string len to 0
    std::string formated_str;

    for (char c : str) {     // loops thru each character in the string
        formated_str += c;
        bkw_cnt--;  // deincrement the count by 1 after the first character which will always be there before checking for needed commas
        if (!bkw_cnt == 0) {
            if(bkw_cnt % 3 == 0) {  // checks if the mod of bkw_cnt / 3 == 0, or remainder is 0
                    formated_str += ',';
            }
        }
    }
    return formated_str;
}