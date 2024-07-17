#include <iostream>
#include <iomanip> // For setw

std::string add_phone_mask(const std::string str);


int main() {

    // Output the text aligned within a field of width 20
    std::cout << "\n\nRuler for reference: \n";
    std::cout << "|<-1     |<-10     |<-20     |<-30     |<-40     |<-50     |<-60     |<-70     |<-80\n";
    std::cout << "|        |         |         |         |         |         |         |         |\n";
    std::cout << "|        |         |         |         |         |         |         |         |\n";
    std::cout << "1. Using setw only ends up with right aligned text at 40 as follows:\n";
    std::cout << std::setw(40) << "Example1-RightAligned_at_40" << std::endl;

    std::cout << "\n2. Example Left aligned text with setw at 40, followed by separate pipe symbol:\n";
    std::cout << std::left << std::setw(40) << "Sample Left Aligned Text w/ set w" << "|" << std::endl;

    std::cout << "\n3. Example right aligned text with setw at 40, followed by separate pipe symbol:\n";
    std::cout << std::right << std::setw(40) << "Example 4" << std::endl;

    std::cout << "\n4. Centered aligned text based on calculation with width at 40, followed by separate pipe symbol:\n";
    int width = 60;
    std::string text = "Center aligned example w/ 34 chars";
    int pad = (width - text.length()) / 2;
    std::cout << std::setw(pad) << "" << text << std::setw(pad) << "|" << std::endl;

    std::cout << "\n5. Try centered a differnt way with pad at right only width at 60, followed by separate pipe symbol:\n";
    text.append(pad, ' ');
    std::cout << std::setw(width) << text << "|" << std::endl;

    std::cout << "\n6. Example Phone Numbers with Masking:\n";
    // Left align text with two columns
    std::string contacts[][2] = {
        { "Smith, John"     , "14095253004" },
        { "Bartiromo, Maria", "8357678123" },
        { "Olsen, Jack", "7556513" }
        };
    
    int name_pad = 30;
    int phone_pad = 20;

    for (auto contact : contacts) {
        std::cout << "\t" << std::left << std::setw(name_pad) << contact[0] << std::right << std::setw(phone_pad) << add_phone_mask(contact[1]) << "\n";
    }
}




std::string add_phone_mask(const std::string str) {
    int len = str.length();

    switch (len)
    {
    case 7:
        return str.substr(0,3) + "-" + str.substr(3,4);
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
