#include<iostream>
#include<string>

int main() {

    std::cout << "Enter Number Your Age Range:\n";
    std::cout << "\tBelow <20: [Enter 1]\n";
    std::cout << "\t20 to <40: [Enter 2]\n";
    std::cout << "\t40 to <60: [Enter 3]\n";
    std::cout << "\t>60:       [Enter 4]\n";
    std::cout << "User Entry Here ->    ";    
    int age_rng;
    std::cin >> age_rng;

    switch (age_rng)
    {
    case 1:
        std::cout << "\nYou entered age range as, Below <20: " << age_rng << "\n";
        break;
    case 2:
        std::cout << "\nYou entered age range as, 20 to <40: " << age_rng << "\n";
        break;
    case 3:
        std::cout << "\nYou entered age range as, 40 to <60: " << age_rng << "\n";
        break;
    case 4:
        std::cout << "\nYou entered age range as, >60:       " << age_rng << "\n";
        break;

    default:
        std::cout << "\nPlease try again b/c your entry was outside range identified\n" << age_rng << "\n";
        break;
    }

    return 0;
}

