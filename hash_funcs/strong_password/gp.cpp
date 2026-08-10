/*

| Item         | Main Program Documentation Notes                            |
|--------------|-------------------------------------------------------------|
| Filename     | gp.cpp                                                      |
| EntryPoint   | main                                                        |
| Purpose      | generate strong passwords                                   |
| Inputs       | varies                                                      |
| Outputs      | generated strong password                                   |
| Dependencies | none                                                        |
| By Name,Date | T.Sciple, 08/10/2026                                        |
Compile iOS/Linux with:
clang++ -std=c++17 -o gp gp.cpp
g++ -std=c++17 -o gp gp.cpp
*/

#include <iostream>
#include <string>    // Required for std::string and std::stoi
#include <cstdlib>   // Required for rand() and srand()
#include <ctime>     // Required for time() to seed the random generator
#include <vector>    // Required for std::vector
#include <random>
#include <algorithm>


char get_random_char(const std::string& char_set) {
    static std::random_device rd;  // Seed for random number generator
    static std::mt19937 gen(rd()); // Mersenne Twister engine
    std::uniform_int_distribution<> dis(0, char_set.size() - 1);
    return char_set[dis(gen)];
}


int get_password(std::string length, bool includeDashes)
{
    int len;
    try {
        len = std::stoi(length);
    } catch (...) {
        std::cout << "Error: Invalid number entered.\n";
        return 1;
    }

    if (len <= 0 || len > 20) {
        std::cout << "Error: Length must be between 1 and 20.\n";
        return 1;
    }
   

    // iPhone-Friendly Pool: Skips confusing letters (O, 0, l, 1, I) 
    // and hides hard-to-find symbols (~, `, \, ^, |).
    std::string safe_chars = "abcdefghijkmnopqrstuvwxyz"
                             "ABCDEFGHJKLMNPQRSTUVWXYZ"
                             "23456789"
                             "!$&*/";
    std::string lowercase = "abcdefghijkmnopqrstuvwxyz";
    std::string uppercase = "ABCDEFGHJKLMNPQRSTUVWXYZ";
    std::string digits = "23456789";
    std::string symbols = "!$&*/";
      
    std::vector<char> password(len);

    // ensure that we have at least one of each required character type
    password[0] = get_random_char(lowercase);
    password[1] = get_random_char(uppercase);
    password[2] = get_random_char(digits);
    password[3] = get_random_char(symbols); 

    std::cout << "\nHere is how the password is constructed:\n"
              << "\tEnsured lowercase symbol: " << password[0] << "\n"
              << "\tEnsured uppercase symbol: " << password[1] << "\n"
              << "\tEnsured digit: " << password[2] << "\n"
              << "\tEnsured symbol: " << password[3] << "\n";

    // Fill the rest of the password length with random characters from the safe set
    for (int i = 4; i < len; i++)
    {
        int index = rand() % safe_chars.length();
        password[i] = safe_chars[index];
    }

    std::cout << "\tUnshuffled password: ";
    for (char c : password) {
        std::cout << c;
    }
    std::cout << std::endl;


    // 2. Initialize a high-quality random number generator (Mersenne Twister)
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(password.begin(), password.end(), g);

    if (!includeDashes) {
        std::cout << "\nFinal Shuffled Generated Password w/o dashes:\n\n\t";
        for (char c : password) {
            std::cout << c;
        }
    } else {
        std::cout << "\nFinal Shuffled Generated Password w/ dashes:\n\n\t";
        for (int i = 0; i < len; ++i) {
            std::cout << password[i];
            if ((i + 1) % 4 == 0 && (i + 1) != len) {
                std::cout << '-';
            }
        }
    }
 
    std::cout << "\n" << std::endl;

    return 0;
}


int main(int argc, char const *argv[])
{
    // Safety Fix 3: Seed the random number generator using the current time.
    // Without this, rand() generates the exact same sequence of passwords every time you run the app.
    srand(static_cast<unsigned int>(time(nullptr)));

    std::cout << "Strong Password Generator:\n\n"
              << "User Input:\n"
              << "\tWould you like to include dashes every 4 characters? (y/n): ";

    char include_dashes;
    std::cin >> include_dashes;

    std::cout << "\tEnter Desired Password length excluding dashes if applicable (10 to 20): ";
    std::string length;
    std::cin >> length;

    bool includeDashes = (include_dashes == 'y' || include_dashes == 'Y');

    int result = get_password(length, includeDashes);

    if (result != 0) {
        std::cout << "Password generation failed due to invalid input.\n";
    }

    return 0;
}
