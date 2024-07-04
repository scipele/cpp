#include<iostream>
#include<string>
//#include <climits>

std::string intFormatWithCommas(int num) {
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

std::string llFormatWithCommas(long long int num) {
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


int main() {

// Integers
    short int a = 32767;  // max size is based on a +/- 32,767 which is 2^16 (2 byte)
    std::cout << "short integer 'short int' a = " << intFormatWithCommas(a) << "\n";

    int b = 2147483647;  // max size is based on a +/- 2,147,483,647 which is (2^32/2 -1)  (4 byte) 
    std::cout << "integer 'int' b = " << llFormatWithCommas(b) << "\n";

    long long int c = 9223372036854775807;  // max size is based on a +/- 9,223,372,036,854,775,807 which is (2^64/2 -1)  (4 byte)
    std::cout << "integer 'long long int' c = " << llFormatWithCommas(c) << "\n";

//characters and strings
    char d = 't';
    std::cout << "characator 'd' = " << d << "\n";    

    //various ways of defining a string
    char str[] = "Tony Sciple\n";
    std::cout << str;
    // Next this will loop thru each character
    for(char ch : str) {
        std::cout << ch << " ";    
    }
    std::cout << "\n";
    // another way of defining a string
    std::string myString = "Hello, world!";

    //this is a string literal
    //these can be useful for multiple line strings that contain newline characters
    // \t = tab,  \n=newline
      std::string multilineString = "(This is an example\n" \
            "\tmultiline\n" \
            "\tstring\n" \
            "\tdefined\n" \
            "\tin code.)";

    std::cout << "Multiline string:\n" << multilineString << std::endl;

//boolean (means true or false)    
    int age = 19;
    bool over_21;
    std::cout << std::boolalpha;
    if (age >= 21) {
        over_21 = true;
    } else {
        over_21 = false;
    }
    std::cout << "Is person over 21 = " << over_21 << "\n";




    return 0;
}