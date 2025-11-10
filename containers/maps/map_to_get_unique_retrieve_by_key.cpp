// Example Usage of a Map

#include<iostream>
#include<map>
#include<string>
#include <iomanip> // For setw

std::string add_phone_mask(const std::string str) {

    int len = str.length();

    switch (len)
    {
    case 11:
        return "+" + str.substr(0,1) + " (" +  str.substr(1,3) + ") " + str.substr(4,3) + "-" + str.substr(7,4);
    case 12:
        return "+" + str.substr(0,2) + " (" +  str.substr(2,3) + ") " + str.substr(5,3) + "-" + str.substr(8,4);
    case 10:
        return "(" +  str.substr(0,3) + ") " + str.substr(3,3) + "-" + str.substr(6,4);
        break;
    default:
        return str;
        break;
    } 
} 

int main() {

    //create a map of strings to string
    std::map<std::string, std::string> phone_list;

    //hard key some items into the phone list
    phone_list["Smith, Sam"] = "214093435001";
    phone_list["Lavergne, Jimbo"] = "14091231346";
    phone_list["Sanford, Mark"] = "14096263642";
    phone_list["Brown, John"] = "14093435001";
    phone_list["Wolf, Zack"] = "4093435001";
    phone_list["Wolf, Zack"] = "4093435002";
    phone_list["Wolf, Zack"] = "4093435003";     //<--- note that the number is overwritten with the last entry
    phone_list["Beal, Joel"] = "093435001"; // <- masking is not applied since no of digits is not 

    //notice that the phone list is printed to the screen in alphabetical order because this when a map is built it is automaticlly sorted as it is defined
    for(auto itr = phone_list.begin(); itr != phone_list.end(); itr++) {
        std::cout << std::left << std::setw(20) << itr->first << add_phone_mask(itr->second) << "\n";
    }
    std::cout << "\n";

    // Count the number of elements in a map
    std::cout << "The number of unique names / keys in the phone_list = " <<  phone_list.size()  << "\n";

    // Lookup the phone number of a particular person
    std::cout << "What is the phone number for Brown, John: " << add_phone_mask(phone_list["Brown, John"]) << "\n";
    std::cout << "What is the phone number for Lavergne, Jimbo: " << add_phone_mask(phone_list["Lavergne, Jimbo"]) << "\n";

    return 0;
}